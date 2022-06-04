#ifndef __BOARDS_ARM_CXD56XX_SPRESENSE_INCLUDE_CXD56_XPT2046
#define __BOARDS_ARM_CXD56XX_SPRESENSE_INCLUDE_CXD56_XPT2046

#include <nuttx/config.h>

#if defined(CONFIG_XPT2046_TOUCH_SENSOR) && defined(CONFIG_SPI)

#include <nuttx/compiler.h>
#include <nuttx/spi/spi.h>
#include <stdint.h>

#ifndef __ASSEMBLY__

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

struct point
{
    int16_t x;
    int16_t y;
    int16_t z;
};

struct xpt2046_handle
{
    uint8_t spi_mode;
    volatile uint8_t isrWake;
    uint8_t cs_pin;
    uint8_t irq_pin;
    int16_t zraw;
    int16_t xraw;
    int16_t yraw;
    uint32_t clk_freq;
    uint32_t msraw;
    FAR struct spi_dev_s *dev;
};

struct xpt2046_handle *board_xpt2046_initialize(int port, uint32_t cs_pin, uint32_t irq_pin, uint32_t frequency);

uint8_t board_xpt2046_uninitialize(struct xpt2046_handle *handle);

uint8_t xpt2046_beginTransaction(struct xpt2046_handle *handle);

uint8_t xpt2046_endTransaction(struct xpt2046_handle *handle);

uint8_t xpt2046_transfer_byte(struct xpt2046_handle *handle, uint8_t data);

uint16_t xpt2046_transfer_word(struct xpt2046_handle *handle, uint16_t data);

uint8_t xpt2046_irq_touched(struct xpt2046_handle *handle);

uint8_t xpt2046_touched(struct xpt2046_handle *handle);

void xpt2046_update(struct xpt2046_handle *handle);

#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif
#endif