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

void interrupt_handler(void) {
    irq_xpt2046_handle->isrWake = true;
}

#endif