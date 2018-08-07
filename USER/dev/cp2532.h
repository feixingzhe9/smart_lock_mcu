#ifndef __CP2532_H_
#define __CP2532_H_
#include <stdio.h>	
#include "sys.h"

#define I2C_CP2532_ADDR     0x2c
//#define I2C_CP2532_READ     (I2C_CP2532_ADDR<<1)
//#define I2C_CP2532_WRITE    (I2C_CP2532_READ + 1)
#define I2C_CP2532_READ     (I2C_CP2532_WRITE + 1)
#define I2C_CP2532_WRITE    (I2C_CP2532_ADDR<<1)

#define I2C_CP2532_QUICK_ADDR           0x24
#define I2C_CP2532_QUICK_READ_ADDR      (I2C_CP2532_QUICK_ADDR<<1)

#define PASS_WORD_LENTH     4

extern u16 touch_key_value_raw;
extern u16 touch_key_value;
extern char psss_word_in_flash[PASS_WORD_LENTH];

u16 get_key_value(void);

u16 read_byte(u8 read_addr);
u16 quick_read(void);
void touch_key_task(void);

#endif

