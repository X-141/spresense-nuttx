#include <nuttx/config.h>
#if defined(CONFIG_SPI)

#include <nuttx/board.h>
#include <nuttx/kmalloc.h>

#include <stdio.h>
#include <debug.h>
#include <errno.h>

#include <nuttx/spi/spi.h>
#include <cxd56_spi.h>
#include <cxd56_clock.h>

#include <arch/board/board.h>
#include <arch/board/cxd56_sample_spi.h>

uint64_t millis(void)
{
    struct timespec tp;

    /* Wait until RTC is available */
    while (g_rtc_enabled == false)
        ;

    if (clock_gettime(CLOCK_MONOTONIC, &tp))
    {
        return 0;
    }

    return (((uint64_t)tp.tv_sec) * 1000 + tp.tv_nsec / 1000000);
}

// Default port value should be 4. (i.e. CONFIG_CXD56_SPI4)
struct sample_spi_handle *board_sample_spi_initialize(int port, uint32_t cs_pin, uint8_t mode, uint8_t bit_order, uint32_t frequency)
{
    FAR struct spi_dev_s *dev = NULL;
    dev = cxd56_spibus_initialize(port);
    if (!dev)
    {
        printf("Failed to initialize spibus.\n");
        return NULL;
    }

    FAR struct sample_spi_handle *handle = NULL;
    handle = (struct sample_spi_handle *)kmm_malloc(sizeof(struct sample_spi_handle));
    if (!handle)
    {
        printf("Failed to allocate spi handle for use.\n");
        return NULL;
    }

    handle->spi_mode = mode;
    handle->bit_order = bit_order;
    handle->clock = frequency;
    handle->cs_pin = cs_pin;
    handle->tirq_pin = 255;
    handle->dev = dev;

    // Go ahead and write to cs pin HIGH to turn it off.
    board_gpio_write(cs_pin, 1);

    return handle;
}

uint8_t board_sample_spi_uninitialize(struct sample_spi_handle *handle)
{
    if (!handle)
    {
        printf("Can't deallocate a null handle.\n");
        return 1;
    }

    if (handle->dev)
    {
        if (SPI_LOCK(handle->dev, false))
        {
            printf("Failed to unlock spi bus.\n");
            return 2;
        }

        handle->dev = NULL;
    }

    handle->isrWake = 1;
    handle->zraw = 0;
    handle->xraw = 0;
    handle->yraw = 0;
    handle->spi_mode = -1;
    handle->bit_order = -1;
    handle->clock = -1;
    handle->cs_pin = 0;
    handle->msraw = 0x80000000;
    free(handle);

    return 0;
}

uint8_t sample_spi_transfer_byte(struct sample_spi_handle *handle, uint8_t data)
{

    if (!handle && handle->dev)
    {
        printf("Can't transfer on null handle or null device.\n");
        return 1;
    }

    uint8_t received = 0;

    SPI_SETBITS(handle->dev, 8);
    SPI_EXCHANGE(handle->dev, (void *)(&data), (void *)(&received), 1);

    return received;
}

uint16_t sample_spi_transfer_word(struct sample_spi_handle *handle, uint16_t data)
{
    if (!handle && handle->dev)
    {
        printf("Can't transfer on null handle or null device.\n");
        return 1;
    }

    union
    {
        uint16_t val;
        struct
        {
            uint8_t hi;
            uint8_t lo;
        };
    } in, out;

    in.val = data;

    SPI_SETBITS(handle->dev, 16);
    SPI_EXCHANGE(handle->dev, (void *)(&in.hi), (void *)(&out.hi), 1);

    return out.val;
}

uint8_t touched(struct sample_spi_handle *handle)
{
    update(handle);
    return (handle->zraw >= Z_THRESHOLD);
}

static int16_t besttwoavg(int16_t x, int16_t y, int16_t z)
{
    int16_t da, db, dc;
    int16_t reta = 0;
    if (x > y)
        da = x - y;
    else
        da = y - x;
    if (x > z)
        db = x - z;
    else
        db = z - x;
    if (z > y)
        dc = z - y;
    else
        dc = y - z;

    if (da <= db && da <= dc)
        reta = (x + y) >> 1;
    else if (db <= da && db <= dc)
        reta = (x + z) >> 1;
    else
        reta = (y + z) >> 1; //    else if ( dc <= da && dc <= db ) reta = (x + y) >> 1;

    return (reta);
}

void update(struct sample_spi_handle *handle)
{
    int16_t data[6];

    if (!handle->isrWake)
        return;

    uint32_t now = millis();
    // printf("now - handle->msraw < MSEC_THRESHOLD; %ld - %ld.\n", now, handle->msraw);
    if (now - handle->msraw < MSEC_THRESHOLD)
        return;

    sample_spi_beginTransaction(handle);

    board_gpio_write(handle->cs_pin, 0);

    sample_spi_transfer_byte(handle, 0xB1);

    int16_t z1 = sample_spi_transfer_word(handle, 0xC1 /* Z2 */) >> 3;
    int z = z1 + 4095;

    int16_t z2 = sample_spi_transfer_word(handle, 0x91 /* X */) >> 3;
    z -= z2;
    if (z >= Z_THRESHOLD)
    {
        sample_spi_transfer_word(handle, 0x91 /* X */); // dummy X measure, 1st is always noisy
        data[0] = sample_spi_transfer_word(handle, 0xD1 /* Y */) >> 3;
        data[1] = sample_spi_transfer_word(handle, 0x91 /* X */) >> 3; // make 3 x-y measurements
        data[2] = sample_spi_transfer_word(handle, 0xD1 /* Y */) >> 3;
        data[3] = sample_spi_transfer_word(handle, 0x91 /* X */) >> 3;
    }
    else
        data[0] = data[1] = data[2] = data[3] = 0; // Compiler warns these values may be used unset on early exit.
    data[4] = sample_spi_transfer_word(handle, 0xD0 /* Y */) >> 3; // Last Y touch power down
    data[5] = sample_spi_transfer_word(handle, 0) >> 3;
    board_gpio_write(handle->cs_pin, 1);

    sample_spi_endTransaction(handle);

    // printf("z=%d  ::  z1=%d,  z2=%d  ", z, z1, z2);
    if (z < 0)
        z = 0;
    if (z < Z_THRESHOLD)
    {
        handle->zraw = 0;
        // if (z < Z_THRESHOLD_INT)
        // {
        //     if (255 != handle->tirq_pin)
        //         handle->isrWake = 0;
        // }
        return;
    }
    handle->zraw = z;

    // Average pair with least distance between each measured x then y
    //printf("    z1=%d,z2=%d  ", z1, z2);
    //printf("p=%d,  %d,%d  %d,%d  %d,%d", handle->zraw,
    //    data[0], data[1], data[2], data[3], data[4], data[5]);
    int16_t x = besttwoavg(data[0], data[2], data[4]);
    int16_t y = besttwoavg(data[1], data[3], data[5]);

    //printf("    %d,%d", x, y);
    //printf("\n");
    if (z >= Z_THRESHOLD)
    {
        handle->msraw = now; // good read completed, set wait
        // switch (rotation)
        switch (1)
        {
        case 0:
            handle->xraw = 4095 - y;
            handle->yraw = x;
            break;
        case 1:
            handle->xraw = x;
            handle->yraw = y;
            break;
        case 2:
            handle->xraw = y;
            handle->yraw = 4095 - x;
            break;
        default: // 3
            handle->xraw = 4095 - x;
            handle->yraw = 4095 - y;
        }
    }
}

uint8_t sample_spi_beginTransaction(struct sample_spi_handle *handle)
{
    if (SPI_LOCK(handle->dev, true))
    {
        printf("Wasn't able to lock spi bus.\n");
        return 1;
    }

    SPI_SETFREQUENCY(handle->dev, handle->clock);
    SPI_SETMODE(handle->dev, (enum spi_mode_e)(handle->spi_mode));

    return 0;
}

uint8_t sample_spi_endTransaction(struct sample_spi_handle *handle)
{
    if (SPI_LOCK(handle->dev, false))
    {
        printf("Wasn't able to unlock spi bus.\n");
        return 1;
    }

    return 0;
}

struct point get_point(struct sample_spi_handle* handle) {
    update(handle);
    struct point p;
    p.x = handle->xraw;
    p.y = handle->yraw;
    p.z = handle->zraw;

    return p;
}

#endif