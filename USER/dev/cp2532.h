#ifndef __CP2532_H_
#define __CP2532_H_
#include "sys.h"

#define I2C_CP2532_ADDR     0x2c
//#define I2C_CP2532_READ     (I2C_CP2532_ADDR<<1)
//#define I2C_CP2532_WRITE    (I2C_CP2532_READ + 1)
#define I2C_CP2532_READ     (I2C_CP2532_WRITE + 1)
#define I2C_CP2532_WRITE    (I2C_CP2532_ADDR<<1)

#define I2C_CP2532_QUICK_ADDR           0x24
#define I2C_CP2532_QUICK_READ_ADDR      (I2C_CP2532_QUICK_ADDR<<1)


u16 read_byte(u8 read_addr);
u16 quick_read(void);

#endif

