#ifndef __BOARDS_ARM_CXD56XX_SPRESENSE_INCLUDE_CXD56_VL53L1X_H
#define __BOARDS_ARM_CXD56XX_SPRESENSE_INCLUDE_CXD56_VL53L1X_H

#include <nuttx/config.h>
#include <nuttx/compiler.h>

#ifndef __ASSEMBLY__

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

#if defined(CONFIG_I2C) && defined(CONFIG_SENSORS_VL53L1X)
int board_vl53l1x_initialize(FAR const char* devpath, int bus, uint8_t address, uint8_t gpio_pin);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif
#endif