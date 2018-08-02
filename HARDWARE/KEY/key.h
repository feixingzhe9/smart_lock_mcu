#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys.h"	 

// 
//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define KEY3 PAin(0)	//PA0  WK_UP
 
#define KEY0  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)//读取按键0

#define HALL1  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)//读取按键0
#define HALL2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)//读取按键0

#define HALL1_ON			0x01
#define HALL2_ON			0x02
	 
#define KEY_MIDDLE	2
#define KEY_RIGHT		1

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数		

uint8_t Hall_Scan( void );
uint8_t is_hall_changed(void);

#ifdef __cplusplus
}
#endif

#endif
