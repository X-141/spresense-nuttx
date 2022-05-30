#ifndef __BOARDS_ARM_CXD56XX_SPRESENSE_INCLUDE_CXD56_SAMPLE_SPI
#define __BOARDS_ARM_CXD56XX_SPRESENSE_INCLUDE_CXD56_SAMPLE_SPI

#include <nuttx/config.h>
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

#if defined(CONFIG_SPI) && defined(CONFIG_SENSORS_SAMPLE_SPI)

#define BUFFER_SPACE 64

// Sourced: spresense-arduino-compatible: spi.h
#define SPI_MODE0 SPIDEV_MODE0  /**< SPI mode 0 */
#define SPI_MODE1 SPIDEV_MODE1  /**< SPI mode 1 */
#define SPI_MODE2 SPIDEV_MODE2  /**< SPI mode 2 */
#define SPI_MODE3 SPIDEV_MODE3  /**< SPI mode 3 */

// Sourced: spresense-arduino-compatible: Arduino.h
#define LSBFIRST 0x0
#define MSBFIRST 0x1

struct sample_spi_handle {
    uint8_t spi_mode;
    uint8_t bit_order;
    uint32_t clock;
    FAR struct spi_dev_s* dev;
};

struct sample_spi_handle* board_sample_spi_initialize(int port, uint8_t mode, uint8_t bit_order, uint32_t frequency);

uint8_t board_sample_spi_uninitialize(struct sample_spi_handle* handle);

uint8_t sample_spi_transfer_byte(struct sample_spi_handle* handle, uint8_t data);

#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif
#endif 