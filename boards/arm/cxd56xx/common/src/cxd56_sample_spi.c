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

#include <arch/board/cxd56_sample_spi.h>

// Default port value should be 4. (i.e. CONFIG_CXD56_SPI4)
struct sample_spi_handle *board_sample_spi_initialize(int port, uint8_t mode, uint8_t bit_order, uint32_t frequency)
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

    handle->dev = dev;
    handle->spi_mode = mode;
    handle->bit_order = bit_order;
    handle->clock = frequency;

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

    handle->spi_mode = -1;
    handle->bit_order = -1;
    handle->clock = -1;
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

    if (SPI_LOCK(handle->dev, true))
    {
        printf("Wasn't able to lock spi bus.\n");
        return 2;
    }

    SPI_SETMODE(handle->dev, (enum spi_mode_e)(handle->spi_mode));
    SPI_SETBITS(handle->dev, 8);
    SPI_SETFREQUENCY(handle->dev, handle->clock);
    SPI_EXCHANGE(handle->dev, (void *)(&data), (void *)(&received), 1);

    if (SPI_LOCK(handle->dev, false))
    {
        printf("Wasn't able to unlock spi bus.\n");
        return 3;
    }

    return received;
}

#endif