#include "cp2532.h"
#include "myiic.h"
#include "can_interface.h"
#include "delay.h"

uint8_t ack_flag[10] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t quick_read_ack_flag[10] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
u16 touch_key_value_raw = 0;
u16 touch_key_value = 0;
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



#define KEY_VALUE_0     (1<<7)
#define KEY_VALUE_1     (1<<8)
#define KEY_VALUE_2     (1<<9)
#define KEY_VALUE_3     (1<<10)
#define KEY_VALUE_4     (1<<4)
#define KEY_VALUE_5     (1<<5)
#define KEY_VALUE_6     (1<<6)
#define KEY_VALUE_7     (1<<3)
#define KEY_VALUE_8     (1<<2)
#define KEY_VALUE_9     (1<<1)
#define KEY_VALUE_A     (1<<11)
#define KEY_VALUE_B     (1<<0)



void set_key_value(u16 key_vlaue)
{
    touch_key_value = key_vlaue;
}

u16 get_key_value(void)
{
    return touch_key_value;
}

/* 判断一次只有一个按键按下，若有多个按键同时按下 返回false， 只有一个按键按下 返回 true 
u16 key_value: 按键值
*/
static bool is_key_valid(u16 key_value)
{
    if(key_value == 0)
    {
        return true;
    }
    else
    {
        u8 key_0 = ((key_value & KEY_VALUE_0) ^ KEY_VALUE_0) ? 1 : 0;
        u8 key_1 = ((key_value & KEY_VALUE_1) ^ KEY_VALUE_1) ? 1 : 0;
        u8 key_2 = ((key_value & KEY_VALUE_2) ^ KEY_VALUE_2) ? 1 : 0;
        u8 key_3 = ((key_value & KEY_VALUE_3) ^ KEY_VALUE_3) ? 1 : 0;
        u8 key_4 = ((key_value & KEY_VALUE_4) ^ KEY_VALUE_4) ? 1 : 0;
        u8 key_5 = ((key_value & KEY_VALUE_5) ^ KEY_VALUE_5) ? 1 : 0;
        u8 key_6 = ((key_value & KEY_VALUE_6) ^ KEY_VALUE_6) ? 1 : 0;
        u8 key_7 = ((key_value & KEY_VALUE_7) ^ KEY_VALUE_7) ? 1 : 0;
        u8 key_8 = ((key_value & KEY_VALUE_8) ^ KEY_VALUE_8) ? 1 : 0;
        u8 key_9 = ((key_value & KEY_VALUE_9) ^ KEY_VALUE_9) ? 1 : 0;
        u8 key_a = ((key_value & KEY_VALUE_A) ^ KEY_VALUE_A) ? 1 : 0;
        u8 key_b = ((key_value & KEY_VALUE_B) ^ KEY_VALUE_B) ? 1 : 0;
               
        u8 check = key_0 + key_1 + key_2 + key_3 + key_4 + key_5 + key_6 + key_7 + key_8 + key_9 + key_a + key_b;
        
        if(check <= 10)
        {
            return false;
        }
        return true;
    }   
}

u16 touch_key_filter(u16 key_value)
{
    static u16 key = 0;
    
    key = key_value;
    
    return  key;
}

#define TOUCH_KEY_PERIOD    80/SYSTICK_PERIOD
void touch_key_task(void)
{
    uint32_t start_tick = 0;
    if(get_tick() - start_tick >= TOUCH_KEY_PERIOD)
    {
        touch_key_value_raw = read_byte(0x31);
        if(is_key_valid(touch_key_value_raw) == true)
        {
            set_key_value( touch_key_filter(touch_key_value_raw) );
        }
        else
        {
            printf("key is invalid ! \r\n");
        }
        if( get_key_value() )
        {
            upload_touch_key_data( get_key_value() );
        }
        start_tick = get_tick();
    }
    
}
