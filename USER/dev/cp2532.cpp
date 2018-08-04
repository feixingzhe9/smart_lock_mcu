#include "cp2532.h"
#include "myiic.h"

uint8_t ack_flag[10] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t quick_read_ack_flag[10] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
u16 read_byte(u8 read_addr)
{
    u8 low_data = 0;
    u8 high_data = 0;
    
    IIC_Start();  
    IIC_Send_Byte(I2C_CP2532_WRITE);	   //发送写命令
    ack_flag[0] = IIC_Wait_Ack();
    IIC_Send_Byte(read_addr);//发送地址
    ack_flag[1] = IIC_Wait_Ack();		
    IIC_Stop();    
    
    IIC_Start();  
	IIC_Send_Byte(I2C_CP2532_READ);	   //发送读命令
	ack_flag[2] = IIC_Wait_Ack();	    	   
    high_data=IIC_Read_Byte(1);	
    low_data=IIC_Read_Byte(0);
    
    IIC_Stop();//产生一个停止条件	    
	return ((high_data<<8) + low_data);
}

u16 quick_read(void)
{
    u8 low_data = 0;
    u8 high_data = 0;
    
    IIC_Start();  
    IIC_Send_Byte(I2C_CP2532_QUICK_READ_ADDR);	   //发送写命令
    quick_read_ack_flag[0] = IIC_Wait_Ack();
    
    high_data=IIC_Read_Byte(1);
    low_data=IIC_Read_Byte(0);
    IIC_Stop();//产生一个停止条件	  
    
	return ((high_data<<8) + low_data);
}
