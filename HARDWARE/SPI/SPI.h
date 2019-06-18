/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@arduino.cc>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
 * Copyright (c) 2014 by Andrew J. Kroll <xxxajk@gmail.com> (atomicity fixes)
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include "sys.h"

#define HIGH			1
#define LOW				0
#define OUTPUT		1

typedef uint32_t size_t;
typedef uint8_t  byte;

#define chipSelectPinRfid2				0x01
#define resetPowerDownPinRfid2		0x02
#define chipSelectPinRfid1				0x03
#define resetPowerDownPinRfid1    0x04

// SPI_HAS_TRANSACTION means SPI has beginTransaction(), endTransaction(),
// usingInterrupt(), and SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

// SPI_HAS_NOTUSINGINTERRUPT means that SPI has notUsingInterrupt() method
#define SPI_HAS_NOTUSINGINTERRUPT 1

// SPI_ATOMIC_VERSION means that SPI has atomicity fixes and what version.
// This way when there is a bug fix you can check this define to alert users
// of your code if it uses better version of this library.
// This also implies everything that SPI_HAS_TRANSACTION as documented above is
// available too.
#define SPI_ATOMIC_VERSION 1

// Uncomment this line to add detection of mismatched begin/end transactions.
// A mismatch occurs if other libraries fail to use SPI.endTransaction() for
// each SPI.beginTransaction().  Connect an LED to this pin.  The LED will turn
// on if any mismatch is ever detected.
//#define SPI_TRANSACTION_MISMATCH_LED 5

#ifndef LSBFIRST
#define LSBFIRST SPI_FirstBit_LSB
#endif
#ifndef MSBFIRST
#define MSBFIRST SPI_FirstBit_MSB
#endif

#define SPI_CLOCK_DIV4 SPI_BaudRatePrescaler_4
#define SPI_CLOCK_DIV16 SPI_BaudRatePrescaler_16
#define SPI_CLOCK_DIV64 SPI_BaudRatePrescaler_64
#define SPI_CLOCK_DIV128 SPI_BaudRatePrescaler_128
#define SPI_CLOCK_DIV2 SPI_BaudRatePrescaler_2
#define SPI_CLOCK_DIV8 SPI_BaudRatePrescaler_8
#define SPI_CLOCK_DIV32 SPI_BaudRatePrescaler_32

#define SPI_MODE0 SPI_Direction_2Lines_FullDuplex
#define SPI_MODE1 SPI_Direction_2Lines_RxOnly
#define SPI_MODE2 SPI_Direction_1Line_Rx
#define SPI_MODE3 SPI_Direction_1Line_Tx

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

// define SPI_AVR_EIMSK for AVR boards with external interrupt pins
#if defined(EIMSK)
#define SPI_AVR_EIMSK  EIMSK
#elif defined(GICR)
#define SPI_AVR_EIMSK  GICR
#elif defined(GIMSK)
#define SPI_AVR_EIMSK  GIMSK
#endif

#define	SPI2_RFID_CS 							PBout(12)
#define SPI2_RFID_CS_READ 				PBin(12)

#define RFID2_RESET_PIN						PCout(7)
#define RFID2_RESET_PIN_READ				PCin(7)

#define	SPI1_RFID_CS 							PAout(4)
#define SPI1_RFID_CS_READ 				PAin(4)

#define RFID1_RESET_PIN						  PCout(5)
#define RFID1_RESET_PIN_READ				PCin(5)

inline void digitalWrite( uint8_t gpio, uint8_t state )
{
    switch (gpio)
    {
        case chipSelectPinRfid2:
            SPI2_RFID_CS = (state == HIGH) ? 1 : 0;
            break;
        case resetPowerDownPinRfid2:
            RFID2_RESET_PIN = (state == HIGH) ? 1 : 0;
            break;
        case chipSelectPinRfid1:
            SPI1_RFID_CS = (state == HIGH) ? 1 : 0;
            break;
        case resetPowerDownPinRfid1:
            RFID1_RESET_PIN = (state == HIGH) ? 1 : 0;
            break;
        default:
            break;
    }

    return;
}

inline uint8_t digitalRead( uint8_t gpio )
{
    switch (gpio)
    {
        case chipSelectPinRfid2:
            return (SPI2_RFID_CS_READ == SET) ? HIGH : LOW;
        case resetPowerDownPinRfid2:
            return (RFID2_RESET_PIN_READ == SET) ? HIGH : LOW;
        case chipSelectPinRfid1:
            return (SPI1_RFID_CS_READ == SET) ? HIGH : LOW;
        case resetPowerDownPinRfid1:
            return (RFID1_RESET_PIN_READ == SET) ? HIGH : LOW;
        default:
            break;
    }

    return 0;
}

inline void pinMode( uint8_t gpio, uint8_t mode )
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    if (gpio == chipSelectPinRfid2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Mode = (mode == OUTPUT) ? GPIO_Mode_Out_PP : GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    else if (gpio == resetPowerDownPinRfid2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = (mode == OUTPUT) ? GPIO_Mode_Out_PP : GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
    }
    else if (gpio == chipSelectPinRfid1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_InitStructure.GPIO_Mode = (mode == OUTPUT) ? GPIO_Mode_Out_PP : GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if (gpio == resetPowerDownPinRfid1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_InitStructure.GPIO_Mode = (mode == OUTPUT) ? GPIO_Mode_Out_PP : GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
    }

    return;
}

class SPISettings {
    public:
        SPISettings(uint32_t clock, uint16_t bitOrder, uint16_t dataMode, SPI_TypeDef *SPIDefine) {
            init_AlwaysInline(clock, bitOrder, dataMode, SPIDefine);
        }
        SPISettings() {
            init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0, SPI1);
        }
    private:
        void init_MightInline(uint32_t clock, uint16_t bitOrder, uint16_t dataMode, SPI_TypeDef *SPIDefine) {
            init_AlwaysInline(clock, bitOrder, dataMode, SPIDefine);
        }
        void init_AlwaysInline(uint32_t clock, uint16_t bitOrder, uint16_t dataMode, SPI_TypeDef *SPIDefine)
            __attribute__((__always_inline__)) {
                GPIO_InitTypeDef GPIO_InitStructure;
                SPI_InitTypeDef  SPI_InitStructure;

                if (SPIDefine == SPI2)
                {
                    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能
                    RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2时钟使能

                    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15复用推挽输出
                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB

                    GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);  //PB13/14/15上拉
                }
                else if (SPIDefine == SPI1)
                {
                    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTA时钟使能
                    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI1时钟使能

                    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PA5/6/7复用推挽输出
                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB

                    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);  //PB5/6/7上拉
                }

                SPI_InitStructure.SPI_Direction = dataMode;//SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
                SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
                SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
                SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为高电平
                SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
                SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
                SPI_InitStructure.SPI_BaudRatePrescaler = clock;//SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
                SPI_InitStructure.SPI_FirstBit = bitOrder;//SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
                SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
                SPI_Init(SPIDefine, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

                SPI_Cmd(SPIDefine, ENABLE); //使能SPI外设
            }
        friend class SPIClass;
};

class SPIClass {
    public:
        SPIClass(SPI_TypeDef *SPIDefine):_SPI(SPIDefine) {};
        SPI_TypeDef *_SPI;
        // Initialize the SPI library
        static void begin();

        // If SPI is used from within an interrupt, this function registers
        // that interrupt with the SPI library, so beginTransaction() can
        // prevent conflicts.  The input interruptNumber is the number used
        // with attachInterrupt.  If SPI is used from a different interrupt
        // (eg, a timer), interruptNumber should be 255.
        static void usingInterrupt(uint8_t interruptNumber);
        // And this does the opposite.
        static void notUsingInterrupt(uint8_t interruptNumber);
        // Note: the usingInterrupt and notUsingInterrupt functions should
        // not to be called from ISR context or inside a transaction.
        // For details see:
        // https://github.com/arduino/Arduino/pull/2381
        // https://github.com/arduino/Arduino/pull/2449

        // Before using SPI.transfer() or asserting chip select pins,
        // this function is used to gain exclusive access to the SPI bus
        // and configure the correct settings.
        inline static void beginTransaction(SPISettings settings) {

        }

        // Write to the SPI bus (MOSI pin) and also receive (MISO pin)
        inline uint8_t transfer(uint8_t data) {
            byte retry = 0;
            while (SPI_I2S_GetFlagStatus(_SPI, SPI_I2S_FLAG_TXE) == RESET)
            {
                retry++;
                if (retry > 200) return 0;
            }
            SPI_I2S_SendData(_SPI, data);
            retry = 0;
            while (SPI_I2S_GetFlagStatus(_SPI, SPI_I2S_FLAG_RXNE) == RESET)
            {
                retry++;
                if (retry > 200) return 0;
            }
            return SPI_I2S_ReceiveData(_SPI);
        }

        // After performing a group of transfers and releasing the chip select
        // signal, this function allows others to access the SPI bus
        inline static void endTransaction(void) {

        }

        // Disable the SPI bus
        static void end();

        // This function is deprecated.  New applications should use
        // beginTransaction() to configure SPI settings.
        inline static void setBitOrder(uint8_t bitOrder) {

        }
        // This function is deprecated.  New applications should use
        // beginTransaction() to configure SPI settings.
        inline static void setDataMode(uint8_t dataMode) {

        }
        // This function is deprecated.  New applications should use
        // beginTransaction() to configure SPI settings.
        inline static void setClockDivider(uint8_t clockDiv) {

        }
        // These undocumented functions should not be used.  SPI.transfer()
        // polls the hardware flag which is automatically cleared as the
        // AVR responds to SPI's interrupt
        inline static void attachInterrupt() { ; }
        inline static void detachInterrupt() { ; }

    private:
        static uint8_t initialized;
        static uint8_t interruptMode; // 0=none, 1=mask, 2=global
        static uint8_t interruptMask; // which interrupts to mask
        static uint8_t interruptSave; // temp storage, to restore state
#ifdef SPI_TRANSACTION_MISMATCH_LED
        static uint8_t inTransactionFlag;
#endif
};

//extern SPIClass SPI_2;
//extern SPIClass SPI_1;
extern SPIClass SPI_2;

#endif
