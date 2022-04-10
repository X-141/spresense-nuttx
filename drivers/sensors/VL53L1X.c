// #include <nuttx/config.h>

// #include <errno.h>
// #include <debug.h>

// #include <nuttx/kmalloc.h>
// #include <nuttx/signal.h>
// #include <nuttx/fs/fs.h>

// #include <nuttx/i2c/i2c_master.h>
// // #include <cxd56_i2c.h>
// // #include <cxd56_pmic.h>

// #include <stdint.h>

// #include <nuttx/sensors/VL53L1X.h>
// #if defined(CONFIG_SENSORS_MYVL53L1X)

// // Using https://github.com/micro-ROS/NuttX/issues/9 as a reference

// #define VL53L1X_I2C_ADDR 0x29
// #define VL53L1X_I2C_SLAVE__DEVICE_ADDRESS 0x0001
// #define VL53L1X_IDENTIFICATION__MODEL_ID 0x010F

// #define TWI_FREQ_100KHZ 100000

// // I2C address length supported
// #define TWI_ADDR_LEN_7_BIT (7)
// #define TWI_ADDR_LEN_10_BIT (10)

// // buffer
// #define BUFFER_LENGTH (32)
// #define TWI_TX_BUF_LEN BUFFER_LENGTH
// #define TWI_RX_BUF_LEN BUFFER_LENGTH

// // return value
// #define TWI_SUCCESS (0)         // success
// #define TWI_DATA_TOO_LONG (1)   // data too long to fit in transmit buffer
// #define TWI_NACK_ON_ADDRESS (2) // received NACK on transmit of address
// #define TWI_NACK_ON_DATA (3)    // received NACK on transmit of data
// #define TWI_OTHER_ERROR (4)     // other error

// #define GPIO_PIN 2

// struct VL53L1X_dev_s
// {
//     FAR struct i2c_master_s *_dev; // I2C interface
//     uint8_t _i2c_address;
//     uint32_t _freq;
//     bool _transmitting;
//     uint16_t _tx_address;
//     uint8_t _tx_addr_len;
//     uint8_t _tx_buf[TWI_TX_BUF_LEN];
//     uint8_t _tx_buf_index;
//     uint8_t _tx_buf_len;
//     uint8_t _rx_buf[TWI_RX_BUF_LEN];
//     uint8_t _rx_buf_index;
//     uint8_t _rx_buf_len;

//     uint8_t _gpio_0;
// };
// int VL53L1X_Init(struct VL53L1X_dev_s *device, uint8_t address);
// void VL53L1X_Off(struct VL53L1X_dev_s *device);
// void VL53L1X_On(struct VL53L1X_dev_s *device);
// int VL53L1X_SetI2CAddress(struct VL53L1X_dev_s *device, uint8_t address);
// int VL53L1X_WrByte(struct VL53L1X_dev_s *device, uint16_t index, uint8_t data);
// // int VL53L1X_I2CWrite(struct VL53L1X_dev_s *device, uint16_t RegisterAddr, uint8_t *pBuffer, uint16_t NumByteToWrite);
// void VL53L1X_BeginTransmission(struct VL53L1X_dev_s *device, uint8_t address);
// uint8_t VL53L1X_EndTransmission(struct VL53L1X_dev_s *device, bool sendStop);
// size_t VL53L1X_WriteToI2C(struct VL53L1X_dev_s *device, const uint8_t *data, size_t quantity);
// size_t VL53L1X_WriteToI2CByte(struct VL53L1X_dev_s *device, uint8_t value);
// uint16_t VL53L1X_SensorID(struct VL53L1X_dev_s *device);

// /**
//  * @brief Called when sensor is first opened.
//  * This method will briefly turn off/on the sensor,
//  * set the address, and initalize the sensor.
//  *
//  * @param address 1 byte address for the I2C sensor
//  * @return int Non-zero value indicates some sort of error has
//  * occured
//  */
// int VL53L1X_Init(struct VL53L1X_dev_s *device, uint8_t address)
// {
//     int status = 0, sensorState = 0;

//     // Turn sensor off/on
//     VL53L1X_Off(device);
//     VL53L1X_On(device);

//     // set the address of the sensor.
//     status = VL53L1X_SetI2CAddress(device, address);
//     if (status < 0)
//     {
//         return status;
//     }

//     if (VL53L1X_SensorID(device) != 0xEACC)
//     {
//         return -1;
//     }

//     return 0;
// }

// /**
//  * @brief Turn sensor off GPIO pin
//  */
// void VL53L1X_Off(struct VL53L1X_dev_s *device)
// {
//     if (device->_gpio_0 >= 0)
//     { // TODO get this from the struct
//         board_gpio_write(device->_gpio_0, 0);
//     }
//     // Sleep for 10 microseconds
//     sleep(0.0000001);
// }

// /**
//  * @brief Turn sensor on GPIO pin
//  */
// void VL53L1X_On(struct VL53L1X_dev_s *device)
// {
//     if (device->_gpio_0 >= 0)
//     { // TODO get this from the struct
//         board_gpio_write(device->_gpio_0, 1);
//     }
//     // Sleep for 10 microseconds
//     sleep(0.0000001);
// }

// /**
//  * @brief Set the I2C address of the VL53L1X sensor
//  *
//  * @param device Struct containing VL53L1X communication data.
//  * @param address New address to assign to the VL53L1X sensor.
//  * @return int Representing the success state. Negative value indicates an error.
//  */
// int VL53L1X_SetI2CAddress(struct VL53L1X_dev_s *device, uint8_t address)
// {
//     int status = 0;

//     // If this is the first time we are setting the address, we will effectively be setting the address
//     // to what we specify.
//     status = VL53L1X_WrByte(device, VL53L1X_I2C_SLAVE__DEVICE_ADDRESS, address >> 1);
//     device->_i2c_address = address;

//     return status;
// }

// /**
//  * @brief Write single byte to VL53L1X sensor
//  *
//  * @param device Struct containing sensor data
//  * @param index
//  * @param data data to write to the sensor
//  * @return int Representing the success state. Negative value indicates an error.
//  */
// int VL53L1X_WrByte(struct VL53L1X_dev_s *device, uint16_t index, uint8_t data)
// {
//     struct i2c_config_s *config;
//     config->address = index;
//     config->addrlen = 7;
//     config->frequency = device->_freq;

//     return i2c_write(device->_dev, config, &data, 1);
//     // return VL53L1X_I2CWrite(device, index, &data, 1);
// }

// /**
//  * @brief Write Byte data to the VL53L1X sensor
//  *
//  * @param device struct containing VL53L1X data
//  * @param RegisterAddr register to write data to
//  * @param pBuffer buffer of data to be written out
//  * @param NumByteToWrite number of bytes within pBuffer
//  * @return int indicates success state. Negative value indicates an error occurred.
//  */
// // int VL53L1X_I2CWrite(struct VL53L1X_dev_s *device, uint16_t RegisterAddr, uint8_t *pBuffer, uint16_t NumByteToWrite)
// // {
// //     // TODO this requires extra parameter.
// //     // VL53L1X_BeginTransmission(device, ((uint8_t)(((device->_i2c_address) >> 1) & 0x7F)));
// //     // sninfo("Writing port number %d.\n", RegisterAddr);
// //     // uint8_t buffer[2];
// //     // buffer[0] = (uint8_t)(RegisterAddr >> 8);
// //     // buffer[1] = (uint8_t)(RegisterAddr & 0xFF);
// //     // VL53L1X_WriteToI2C(device, buffer, 2);
// //     // for (uint16_t i = 0; i < NumByteToWrite; i++)
// //     // {
// //     //     VL53L1X_WriteToI2CByte(device, pBuffer[i]);
// //     // }
// //     // VL53L1X_EndTransmission(device, true);
// //     struct i2c_config_s *config;
// //     config->address = RegisterAddr;
// //     config->addrlen = 7;
// //     config->frequency = device->_freq;
// //     return i2c_write(device->_dev, config, pBuffer, NumByteToWrite);
// //     // return 0;
// // }

// /**
//  * @brief Write byte data to VL53L1X data
//  *
//  * @param device struct containing VL53L1X information.
//  * @param data pointer to array of data to write out
//  * @param quantity number of bytes in the data
//  * @return size_t number of bytes written out.
//  */
// size_t VL53L1X_WriteToI2C(struct VL53L1X_dev_s *device, const uint8_t *data, size_t quantity)
// {

//     if (device->_transmitting)
//     {
//         for (size_t i = 0; i < quantity; ++i)
//             VL53L1X_WriteToI2CByte(device, data[i]);
//     }
//     else
//     {
//         sninfo("Error we are not in a transmission state.\n");
//     }

//     return quantity;
// }

// /**
//  * @brief Write out single byte of data
//  *
//  * @param device struct containing VL53L1X data
//  * @param value value to be written out
//  * @return size_t number of bytes written out. Should always be 1 if no errors occurred.
//  */
// size_t VL53L1X_WriteToI2CByte(struct VL53L1X_dev_s *device, uint8_t value)
// {
//     if (device->_transmitting)
//     {
//         // in master transmitter mode
//         // don't bother if buffer is full
//         // TODO: HANDLE THIS PLS
//         // if (_tx_buf_len >= TWI_TX_BUF_LEN) {
//         //     setWriteError();
//         //     return 0;
//         // }
//         // put byte in tx buffer
//         device->_tx_buf[device->_tx_buf_index] = value;
//         ++(device->_tx_buf_index);
//         // update amount in buffer
//         device->_tx_buf_len = device->_tx_buf_index;
//     }
//     else
//     {
//         // in slave send mode
//         // reply to master
//         sninfo("ERROR: I2C slave mode not supported on CXD5602\n");
//     }
//     return 1;
// }


// /**
//  * @brief Set Transmission state to true and reinitialize internal data
//  * to prepare data transfer
//  *
//  * @param device struct containing VL53L1X sensor information
//  * @param address Recieving address.
//  */
// void VL53L1X_BeginTransmission(struct VL53L1X_dev_s *device, uint8_t address)
// {
//     // indicate that we are transmitting
//     device->_transmitting = 1;
//     // set address of targeted slave
//     device->_tx_address = address;
//     // reset tx buffer iterator vars
//     device->_tx_buf_index = 0;
//     device->_tx_buf_len;
// }

// /**
//  * @brief Begin transmitting I2C data to VL53L1X sensor
//  *
//  * @param device struct containing VL53L1X data.
//  * @param sendStop after transmission, send stop signal to the line
//  * @return uint8_t indicates success state.
//  */
// uint8_t VL53L1X_EndTransmission(struct VL53L1X_dev_s *device, bool sendStop)
// {
//     struct i2c_msg_s msg;
//     unsigned int flags = 0;
//     int ret;

//     if (!device->_dev || !device->_transmitting)
//         return TWI_OTHER_ERROR;

//     if (device->_tx_addr_len == TWI_ADDR_LEN_10_BIT)
//     {
//         flags |= I2C_M_TEN;
//     }

//     if (!sendStop)
//     {
//         flags |= I2C_M_NOSTOP;
//     }

//     // Setup for the transfer
//     msg.frequency = device->_freq;
//     msg.addr = device->_tx_address;
//     msg.flags = flags;
//     msg.buffer = device->_tx_buf;
//     msg.length = device->_tx_buf_len;

//     // Then perform the transfer.
//     ret = I2C_TRANSFER(device->_dev, &msg, 1);
//     // reset tx buffer iterator vars
//     device->_tx_buf_index = 0;
//     device->_tx_buf_len = 0;
//     // indicate that we are done transmitting
//     device->_transmitting = false;

//     if (ret == -ENODEV)
//     {
//         // device not found
//         return TWI_NACK_ON_ADDRESS;
//     }
//     return (ret < 0) ? TWI_OTHER_ERROR : TWI_SUCCESS;
// }


// uint16_t VL53L1X_SensorID(struct VL53L1X_dev_s *device)
// {
//     uint8_t status = 0;
//     uint16_t tmp = 0;

//     status = VL53L1X_RdWord(device, VL53L1X_IDENTIFICATION__MODEL_ID, &tmp);
//     return tmp;
// }

// uint16_t VL53L1X_RdWord(struct VL53L1X_dev_s *device, uint16_t index, uint16_t *data)
// {
//     int status;
//     uint8_t buffer[2] = {0, 0};

//     status = VL53L1X_I2CRead(device->_i2c_address, index, buffer, 2);
//     if (!status)
//     {
//         *data = (buffer[0] << 8) + buffer[1];
//     }
//     return status;
// }

// uint16_t VL53L1X_I2CRead(struct VL53L1X_dev_s *device, uint16_t RegisterAddr, uint8_t *pBuffer, uint16_t NumByteToRead)
// {
//     int status = 0;
// //     // Loop until the port is transmitted correctly
// //     do
// //     {
// // #ifdef DEBUG_MODE
// //         sinfo("Beginning transmission to %d.\n", ((DeviceAddr) >> 1) & 0x7F);
// // #endif
// //         VL53L1X_BeginTransmission(device, ((uint8_t)(((device->_i2c_address) >> 1) & 0x7F)));
// // #ifdef DEBUG_MODE
// //         sinfo("Writing port number %d.\n", RegisterAddr);
// // #endif
// //         uint8_t buffer[2];
// //         buffer[0] = (uint8_t)(RegisterAddr >> 8);
// //         buffer[1] = (uint8_t)(RegisterAddr & 0xFF);
// //         VL53L1X_WriteToI2C(device, buffer, 2);
// //         status = VL53L1X_EndTransmission(device, false);
// //     } while (status != 0);

//     // i2c_writeread()

//     uint8_t buffer[2];
//     buffer[0] = (uint8_t)(RegisterAddr >> 8);
//     buffer[1] = (uint8_t)(RegisterAddr & 0xFF);

//     FAR struct i2c_config_s *config;
//     config->address = device->_i2c_address;
//     config->addrlen = 7;
//     config->frequency = device->_freq;

//     // dev_i2c->requestFrom(((uint8_t)(((device->_i2c_address) >> 1) & 0x7F)), (byte)NumByteToRead);
//     // i2c_read(device->_dev, config, pBuffer, NumByteToRead);

//     return i2c_writeread(device->_dev, config, buffer, 2, pBuffer, NumByteToRead);

//     // int i = 0;
//     // while (dev_i2c->available())
//     // {
//     //     pBuffer[i] = dev_i2c->read();
//     //     i++;
//     // }

//     // return 0;
// }

// static int VL53L1X_open(FAR struct file *filep);
// static int VL53L1X_close(FAR struct file *filep);
// static ssize_t VL53L1X_read(FAR struct file *filep, FAR char *buffer, size_t buflen);
// static int VL53L1X_ioctl(FAR struct file *filep, int cmd, unsigned long arg);

// static const struct file_operations g_VL53L1Xops =
//     {
//         VL53L1X_open,
//         VL53L1X_close,
//         VL53L1X_read,
//         0,
//         0,
//         VL53L1X_ioctl,
//         0,
//         0};

// static int VL53L1X_register(FAR const char *devPath, int port)
// {
//     FAR struct VL53L1X_dev_s *priv;
//     int ret;

//     priv = (FAR struct VL53L1X_dev_s *)kmm_malloc(sizeof(struct VL53L1X_dev_s));
//     if (!priv)
//     {
//         snerr("ERROR: Failed to allocate VL53L1X structure instance.\n");
//         return -ENOMEM;
//     }

//     FAR struct i2c_master_s *i2c = cxd56_i2cbus_initialize(port);
//     if (!i2c)
//     {
//         return -ENODEV;
//     }

//     priv->_dev = i2c;
//     priv->_i2c_address = VL53L1X_I2C_ADDR;
//     priv->_freq = TWI_FREQ_100KHZ;
//     priv->_transmitting = false;
//     priv->_tx_address = 0;
//     priv->_tx_addr_len = TWI_ADDR_LEN_7_BIT;
//     priv->_tx_buf_index = 0;
//     priv->_tx_buf_len = 0;
//     priv->_rx_buf_index = 0;
//     priv->_rx_buf_len = 0;

//     priv->_gpio_0 = GPIO_PIN;

//     ret = register_driver(devPath, &g_VL53L1Xops, 0666, priv);
//     if (ret < 0)
//     {
//         snerr("ERROR: Failed to register driver %d.\n", ret);
//         kmm_free(priv);
//     }

//     return ret;
// }

// static int VL53L1X_open(FAR struct file *filep)
// {
//     FAR struct inode *inode = filep->f_inode;
//     FAR struct VL53L1X_dev_s *priv = inode->i_private;

//     VL53L1X_Init(priv, priv->_i2c_address);
//     sninfo("Device is available.\n");
//     return OK;
// }

// static int VL53L1X_close(FAR struct file *filep)
// {
//     // FAR struct inode *inode = filep->f_inode;
//     // FAR struct VL53L1X_dev_s *priv = inode->i_private;

//     // cxd56_i2cbus_uninitialize(priv->_dev);
//     return OK;
// }

// static ssize_t VL53L1X_read(FAR struct file *filep, FAR char *buffer, size_t buflen);
// static int VL53L1X_ioctl(FAR struct file *filep, int cmd, unsigned long arg);

// #endif