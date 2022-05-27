#ifndef __BOARDS_ARM_CXD56XX_SPRESENSE_INCLUDE_CXD56_SAMPLE_SPI
#define __BOARDS_ARM_CXD56XX_SPRESENSE_INCLUDE_CXD56_SAMPLE_SPI

#include <nuttx/config.h>
#include <nuttx/compiler.h>
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

#if defined(CONFIG_SPI)
int board_sample_spi_initialize(FAR const char* devpath, int port);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif
#endif 