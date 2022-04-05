#include <nuttx/config.h>

#include <errno.h>
#include <debug.h>

#include <nuttx/kmalloc.h>
#include <nuttx/signal.h>
#include <nuttx/fs/fs.h>
#include <nuttx/sensors/mySensor.h>

#if defined(CONFIG_SENSORS_MYSENSOR)

struct mySensor_dev_s
{
    uint8_t myValue;
    bool isValid;
};

/* Character driver methods */

static int mySensor_open(FAR struct file *filep);
static int mySensor_close(FAR struct file *filep);
static ssize_t mySensor_read(FAR struct file *filep, FAR char *buffer,
                             size_t buflen);
static int mySensor_ioctl(FAR struct file *filep, int cmd, unsigned long arg);

// This struct links the function with action over the file,
// it's very important to respect the positions.
static const struct file_operations g_mySensorops =
    {
        mySensor_open,  // open
        mySensor_close, // close
        mySensor_read,  // read
        0,              // write
        0,              // seek
        mySensor_ioctl, // ioctl
        0,              // poll
        0,              // unlink
};

int mySensor_register(FAR const char *devpath)
{
    FAR struct mySensor_dev_s *priv;
    int ret;

    priv = (FAR struct mySensor_dev_s *)kmm_malloc(sizeof(struct mySensor_dev_s));
    if (!priv)
    {
        snerr("ERROR: Failed to allocate instance\n");
        return -ENOMEM;
    }

    priv->isValid = true;
    priv->myValue = 0;

    ret = register_driver(devpath, &g_mySensorops, 0666, priv);
    if (ret < 0)
    {
        snerr("ERROR: Failed to register driver: %d\n", ret);
        kmm_free(priv);
    }

    return ret;
}

static int mySensor_open(FAR struct file *filep)
{
    sninfo("Device correctly opened!\n");
    return OK;
}

static int mySensor_close(FAR struct file *filep)
{
    sninfo("Device was closed!\n");
    return OK;
}

static ssize_t mySensor_read(FAR struct file *filep, FAR char *buffer,
                             size_t buflen)
{
    FAR struct inode *inode = filep->f_inode;
    FAR struct mySensor_dev_s *priv = inode->i_private;
    FAR struct mySensor_s *ptr = (FAR struct mySensor_s *)buffer;

    (*ptr).val = priv->myValue;
    return sizeof(struct mySensor_s);
}

static int mySensor_ioctl(FAR struct file *filep, int cmd, unsigned long arg)
{
    FAR struct inode *inode = filep->f_inode;
    FAR struct mySensor_dev_s *priv = inode->i_private;

    switch (cmd)
    {
    case 1: // INCREMENT
    {
        sninfo("Incrementing value.\n");
        priv->myValue++;
    }
    break;
    }
}

#endif