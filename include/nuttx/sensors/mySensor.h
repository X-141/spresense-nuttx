#ifndef __INCLUDE_NUTTX_SENSORS_MYSENSOR_H
#define __INCLUDE_NUTTX_SENSORS_MYSENSOR_H

#include <nuttx/config.h>
#include <nuttx/sensors/ioctl.h>

#if defined(CONFIG_SENSORS_MYSENSOR)

/********************************************************************************************
 * Pre-processor Definitions
 ********************************************************************************************/


/* Configuration ****************************************************************************/
/* Prerequisites:
 *
 * CONFIG_SENSORS_MYSENSOR
 *   Enables support for the mySensor driver
 */

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct i2c_master_s;

struct mySensor_s
{
  uint8_t val;
};

/********************************************************************************************
 * Public Function Prototypes
 ********************************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Name: mySensor_register
 *
 * Description:
 *   Register the mySensor character device as 'devpath'
 *
 * Input Parameters:
 *   devpath - The full path to the driver to register. E.g., "/dev/mySensor"
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int mySensor_register(FAR const char *devpath);


#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif
#endif 