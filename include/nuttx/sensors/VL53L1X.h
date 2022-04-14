#ifndef __INCLUDE_NUTTX_SENSORS_MYVL53L1X_H
#define __INCLUDE_NUTTX_SENSORS_MYVL53L1X_H

#include <nuttx/config.h>
#include <nuttx/sensors/ioctl.h>

#if defined(CONFIG_SENSORS_MYVL53L1X) 

struct i2c_master_s;

struct VL53L1X_s
{
  uint16_t distance;
};

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

int VL53L1X_register(FAR const char *devpath, FAR struct i2c_master_s* i2c);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif
#endif