#include <nuttx/config.h>

#if defined(CONFIG_XPT2046_TOUCH_SENSOR) && defined(CONFIG_SPI)

#include <nuttx/spi/spi.h>
#include <nuttx/board.h>
#include <nuttx/kmalloc.h>

#include <arch/board/board.h>
#include <arch/board/cxd56_xpt2046.h>
#include <cxd56_spi.h>

#include <stdbool.h>
#include <stdio.h>

#define HIGH (1)
#define LOW (0)

// Used internally
#define Z_THRESHOLD (400)
#define Z_THRESHOLD_INT (75)
#define MSEC_THRESHOLD (3)

static uint64_t millis(void)
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

void interrupt_handler(void);
struct xpt2046_handle *irq_xpt2046_handle;

struct xpt2046_handle *board_xpt2046_initialize(int port, uint32_t cs_pin, uint32_t irq_pin, uint32_t frequency)
{
    FAR struct spi_dev_s *dev = NULL;
    dev = cxd56_spibus_initialize(port);
    if (!dev)
    {
        printf("Unable to initialize SPI bus for XPT2046 initialization.\n");
        return NULL;
    }

    FAR struct xpt2046_handle *handle = NULL;
    handle = (struct xpt2046_handle *)kmm_malloc(sizeof(struct xpt2046_handle));
    if (!handle)
    {
        printf("Failed to allocate XPT2046 handle.\n");
        return NULL;
    }

    handle->spi_mode = SPIDEV_MODE0;
    handle->isrWake = false;
    handle->cs_pin = cs_pin;
    handle->irq_pin = irq_pin;
    handle->zraw = handle->xraw = handle->yraw = 0;
    handle->clk_freq = frequency;
    handle->msraw = 0;
    handle->dev = dev;

    board_gpio_write(handle->cs_pin, HIGH);

    if (handle->irq_pin != 255)
    {
        int irq = board_gpio_intconfig(handle->irq_pin, INT_FALLING_EDGE, true, (xcpt_t)interrupt_handler);
        if (irq < 0)
        {
            printf("Unable to configure interrupt pin for XPT2046 driver.");
            board_xpt2046_uninitialize(handle);
            return NULL;
        }
        usleep(1000);
        board_gpio_int(handle->irq_pin, true);
        irq_xpt2046_handle = handle;
    }

    return handle;
}

uint8_t board_xpt2046_uninitialize(struct xpt2046_handle *handle)
{
    if (!handle)
    {
        printf("xpt2046 handle passed is NULL. Failed to uinitialize.\n");
        return 1;
    }

    if (handle->dev)
    {
        if (SPI_LOCK(handle->dev, false))
        {
            printf("Failed to unlock spi bus currently belonging to the xpt2046 handle.\n");
            return 2;
        }

        handle->dev = NULL;
    }

    if (handle->irq_pin != 255 && board_gpio_int(handle->irq_pin, false) < 0)
    {
        printf("Failed to disable interrupt pin attached to xpt2046 handle.\n");
        return 3;
    }
    if (board_gpio_intconfig(handle->irq_pin, 0, false, NULL) < 0)
    {
        printf("Failed to disable interrupt attached to xpt2046 handle.\n");
        return 4;
    }

    handle->spi_mode = 0;
    handle->isrWake = 0;
    handle->cs_pin = 0;
    handle->irq_pin = 0;
    handle->zraw = handle->xraw = handle->yraw = 0;
    handle->clk_freq = 0;
    handle->msraw = 0;

    free(handle);

    return 0;
}

void interrupt_handler(void)
{
    irq_xpt2046_handle->isrWake = true;
}

uint8_t xpt2046_beginTransaction(struct xpt2046_handle *handle)
{

    if (!handle || !handle->dev)
    {
        printf("xpt2046 handle is either NULL or underlying SPI bus device has not been initialized.\n");
        return 1;
    }

    if (SPI_LOCK(handle->dev, true))
    {
        printf("xpt2046 handle is unable to get a handle of the SPI bus.\n");
        return 2;
    }

    SPI_SETFREQUENCY(handle->dev, handle->clk_freq);
    SPI_SETMODE(handle->dev, (enum spi_mode_e)(handle->spi_mode));

    return 0;
}

uint8_t xpt2046_endTransaction(struct xpt2046_handle *handle)
{
    if (!handle || !handle->dev)
    {
        printf("xpt2046 handle is either NULL or underlying SPI bus device has not been initialized.\n");
        return 1;
    }

    if (SPI_LOCK(handle->dev, false))
    {
        printf("xpt2046 handle is unable to unlock handle of the SPI bus.\n");
        return 2;
    }

    return 0;
}

uint8_t xpt2046_transfer_byte(struct xpt2046_handle *handle, uint8_t data)
{
    if (!handle || !handle->dev)
    {
        printf("Can't transfer on null handle or null device.\n");
        return 1;
    }

    uint8_t received = 0;

    SPI_SETBITS(handle->dev, 8);
    SPI_EXCHANGE(handle->dev, (void *)(&data), (void *)(&received), 1);

    return received;
}

uint16_t xpt2046_transfer_word(struct xpt2046_handle *handle, uint16_t data)
{
    if (!handle || !handle->dev)
    {
        printf("Can't transfer on null handle or null device.\n");
        return 1;
    }

    uint16_t received = 0;

    SPI_SETBITS(handle->dev, 16);
    SPI_EXCHANGE(handle->dev, (void *)(&data), (void *)(&received), 1);

    return received;
}

uint8_t xpt2046_irq_touched(struct xpt2046_handle *handle)
{
    return handle->isrWake;
}

uint8_t xpt2046_touched(struct xpt2046_handle *handle)
{
    xpt2046_update(handle);
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

void xpt2046_update(struct xpt2046_handle *handle)
{
    int16_t data[6];

    if (!handle->isrWake)
        return;

    uint32_t now = millis();
    // printf("now - handle->msraw < MSEC_THRESHOLD; %ld - %ld.\n", now, handle->msraw);
    if (now - handle->msraw < MSEC_THRESHOLD)
        return;

    xpt2046_beginTransaction(handle);

    board_gpio_write(handle->cs_pin, 0);

    xpt2046_transfer_byte(handle, 0xB1);

    int16_t z1 = xpt2046_transfer_word(handle, 0xC1 /* Z2 */) >> 3;
    int z = z1 + 4095;

    int16_t z2 = xpt2046_transfer_word(handle, 0x91 /* X */) >> 3;
    z -= z2;
    if (z >= Z_THRESHOLD)
    {
        xpt2046_transfer_word(handle, 0x91 /* X */); // dummy X measure, 1st is always noisy
        data[0] = xpt2046_transfer_word(handle, 0xD1 /* Y */) >> 3;
        data[1] = xpt2046_transfer_word(handle, 0x91 /* X */) >> 3; // make 3 x-y measurements
        data[2] = xpt2046_transfer_word(handle, 0xD1 /* Y */) >> 3;
        data[3] = xpt2046_transfer_word(handle, 0x91 /* X */) >> 3;
    }
    else
        data[0] = data[1] = data[2] = data[3] = 0;                 // Compiler warns these values may be used unset on early exit.
    data[4] = xpt2046_transfer_word(handle, 0xD0 /* Y */) >> 3; // Last Y touch power down
    data[5] = xpt2046_transfer_word(handle, 0) >> 3;
    board_gpio_write(handle->cs_pin, 1);

    xpt2046_endTransaction(handle);

    // printf("z=%d  ::  z1=%d,  z2=%d  ", z, z1, z2);
    if (z < 0)
        z = 0;
    if (z < Z_THRESHOLD)
    {
        handle->zraw = 0;
        if (z < Z_THRESHOLD_INT)
        {
            if (handle->irq_pin != 255)
                handle->isrWake = 0;
        }
        return;
    }
    handle->zraw = z;

    // Average pair with least distance between each measured x then y
    // printf("    z1=%d,z2=%d  ", z1, z2);
    // printf("p=%d,  %d,%d  %d,%d  %d,%d", handle->zraw,
    //    data[0], data[1], data[2], data[3], data[4], data[5]);
    int16_t x = besttwoavg(data[0], data[2], data[4]);
    int16_t y = besttwoavg(data[1], data[3], data[5]);

    // printf("    %d,%d", x, y);
    // printf("\n");
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

#endif