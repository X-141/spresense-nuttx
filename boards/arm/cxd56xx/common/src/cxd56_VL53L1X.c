#include <nuttx/config.h>
#include <nuttx/board.h>

#include <stdio.h>
#include <debug.h>
#include <errno.h>

#include "cxd56_i2c.h"

#if defined(CONFIG_I2C) && defined(CONFIG_SENSORS_VL53L1X)

#include <nuttx/sensors/vl53l1x.h>

int board_vl53l1x_initialize(FAR const char *devpath, int bus)
{
    int ret;
    FAR struct i2c_master_s *i2c;

    printf("Initializing vl53l1x 2.\n");

    i2c = cxd56_i2cbus_initialize(bus);
    if (!i2c)
    {
        printf("ERROR: Failed to initialize i2c %d.\n", bus);
        return -ENODEV;
    }
    printf("cxd56 bus initialized.\n");
    
    ret = vl53l1x_register(devpath, i2c);
    if (ret < 0)
    {
        printf("Error registering vl53l1x.\n");
    }

    return ret;
}

#endif