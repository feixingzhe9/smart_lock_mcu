#include "cp2532.h"
#include "myiic.h"
#include "can_interface.h"
#include "delay.h"

uint8_t ack_flag[10] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t quick_read_ack_flag[10] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
u16 cp2532_test = 0;
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

static can_message_t touch_key_message_ram;
static can_message_t *touch_key_message = &touch_key_message_ram;

static void upload_touch_key_data(u16 key_value)
{
    uint8_t key_value_low = key_value&0xff;
    uint8_t key_value_high = key_value>>8;
    touch_key_message->id = 0x1aa02188;
    touch_key_message->data[0] = 0;

    touch_key_message->data_len = 3;
    touch_key_message->data[1] = key_value_high;
    touch_key_message->data[2] = key_value_low;
    
    can.can_send( touch_key_message );
    
    return;
}




#define TOUCH_KEY_PERIOD    80/SYSTICK_PERIOD
void touch_key_task(void)
{
    uint32_t start_tick = 0;
    if(get_tick() - start_tick >= TOUCH_KEY_PERIOD)
    {
        cp2532_test = read_byte(0x31);
        if(cp2532_test)
        {
            upload_touch_key_data(cp2532_test);
        }
        start_tick = get_tick();
    }
    
}
