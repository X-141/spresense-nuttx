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


// Sourced: spresense-arduino-compatible: spi.h
#define SPI_MODE0 SPIDEV_MODE0  /**< SPI mode 0 */
// TODO Remove these guys and remove the defined var.
#define SPI_MODE1 SPIDEV_MODE1  /**< SPI mode 1 */
#define SPI_MODE2 SPIDEV_MODE2  /**< SPI mode 2 */
#define SPI_MODE3 SPIDEV_MODE3  /**< SPI mode 3 */

// Sourced: spresense-arduino-compatible: Arduino.h
#define LSBFIRST 0x0
#define MSBFIRST 0x1

struct point
{
    int16_t x;
    int16_t y;
    int16_t z;
};

struct xpt2046_handle
{
    uint8_t spi_mode;
    uint8_t bit_order;
    volatile uint8_t isrWake;
    int16_t zraw;
    int16_t xraw;
    int16_t yraw;
    uint32_t clock;
    uint32_t cs_pin;
    uint32_t tirq_pin;
    uint32_t msraw;
    FAR struct spi_dev_s *dev;
};

struct xpt2046_handle *board_xpt2046_initialize(int port, uint32_t cs_pin, uint32_t irq_pin, uint8_t mode, uint8_t bit_order, uint32_t frequency);

uint8_t board_xpt2046_uninitialize(struct xpt2046_handle *handle);

#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif
#endif