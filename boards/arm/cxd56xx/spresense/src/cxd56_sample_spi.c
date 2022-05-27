#include <nuttx/config.h>
#include <nuttx/board.h>

#include <stdio.h>
#include <debug.h>
#include <errno.h>

#if defined(CONFIG_SPI)

#include <nuttx/spi/spi.h>
#include <cxd56_spi.h>

int board_sample_spi_initialize(FAR const char *devpath, int port)
{
    // TODO
    FAR struct spi_dev_s * dev;
    dev = cxd56_spibus_initialize(port);
    if (!dev) {
        printf("Failed to initialize spibus.\n");
    }
    (void) SPI_LOCK(dev, false);
    // int ret;
    // FAR struct i2c_master_s *i2c;

    // i2c = cxd56_i2cbus_initialize(bus);
    // if (!i2c)
    // {
    //     snerr("ERROR: Failed to initialize i2c %d.\n", bus);
    //     return -ENODEV;
    // }

    // ret = vl53l1x_register(devpath, i2c, address, gpio_pin);
    // if (ret < 0)
    // {
    //     snerr("Error registering vl53l1x.\n");
    // }

    // return ret;
    return 0;
}

#endif 