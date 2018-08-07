#include "cp2532.h"
#include "myiic.h"
#include "can_interface.h"
#include "delay.h"
#include <string.h>
#include "lock.h"

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

void set_key_value(u16 key_vlaue)
{
    touch_key_value = key_vlaue;
}

u16 get_key_value(void)
{
    return touch_key_value;
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


static u8 get_true_key_value(u16 key_value)
{
    u8 key_true_value = 0;
    
    if(key_value & KEY_VALUE_0)
    {
        key_true_value = '0';
    }
    else if(key_value & KEY_VALUE_1)
    {
        key_true_value = '1';
    }
    else if(key_value & KEY_VALUE_2)
    {
        key_true_value = '2';
    }
    else if(key_value & KEY_VALUE_3)
    {
        key_true_value = '3';
    }
    else if(key_value & KEY_VALUE_4)
    {
        key_true_value = '4';
    }
    else if(key_value & KEY_VALUE_5)
    {
        key_true_value = '5';
    }
    else if(key_value & KEY_VALUE_6)
    {
        key_true_value = '6';
    }
    else if(key_value & KEY_VALUE_7)
    {
        key_true_value = '7';
    }
    else if(key_value & KEY_VALUE_8)
    {
        key_true_value = '8';
    }
    else if(key_value & KEY_VALUE_9)
    {
        key_true_value = '9';
    }
    else if(key_value & KEY_VALUE_A)
    {
        key_true_value = 'a';
    }
    else if(key_value & KEY_VALUE_B)
    {
        key_true_value = 'b';
    }
    else
    {
        return 0;
    }
    
    return key_true_value;
}


#define PASS_WORD_LENTH     4
struct pass_word_t
{
    u16 pass_word;
    u32 start_tick;
};

struct pass_word_info_t
{
    pass_word_t pass_word_buf[PASS_WORD_LENTH];
    u8 lenth;
};


pass_word_info_t pass_word_info_t_ram = {0};
pass_word_info_t *pass_word_info = &pass_word_info_t_ram;




static void shift_letf_pass_word(void)
{
    if(pass_word_info->lenth > 0)
    {
        for(u8 i = 1; i <  pass_word_info->lenth; i++)
        {
            memcpy( &(pass_word_info->pass_word_buf[i - 1]), &(pass_word_info->pass_word_buf[i]), sizeof(pass_word_t));
        }
        pass_word_info->lenth--;
    }    
}

static void clear_pass_word(void)
{
    pass_word_t pass_word;
    pass_word.pass_word = 0;
    pass_word.start_tick = 0;
    
    for(u8 i = 1; i <  pass_word_info->lenth; i++)
    {
        memcpy( &(pass_word_info->pass_word_buf[i]), &pass_word, sizeof(pass_word_t));
    }
    pass_word_info->lenth = 0;
}

static void insert_one_pass_word(pass_word_t *key_info)
{
    if(pass_word_info->lenth < PASS_WORD_LENTH)
    {
        memcpy( &(pass_word_info->pass_word_buf[pass_word_info->lenth]), key_info, sizeof(pass_word_t));
        pass_word_info->lenth++;
    }
    else
    {
        shift_letf_pass_word();
        memcpy(&(pass_word_info->pass_word_buf[pass_word_info->lenth]), key_info, sizeof(pass_word_t));
        pass_word_info->lenth++;
    }
}

void pass_work_proc(void)
{
    char password[PASS_WORD_LENTH];
    char psss_word_test[PASS_WORD_LENTH] = {'1', '2', '3', '4'};
    if(pass_word_info->lenth == PASS_WORD_LENTH)
    {
        for(u8 i = 0; i < pass_word_info->lenth; i++)
        {
            password[i] = pass_word_info->pass_word_buf[i].pass_word;
        }
    }
    else
    {
        return ;
    }
    
    for(u8 i = 0; i < PASS_WORD_LENTH; i++)
    {
        if(password[i] != psss_word_test[i])
        {
            return ;
        }            
    }
    
    
    // ----   get right password here  ----//
    printf("get right password");
    clear_pass_word();
    lock_1.is_need_to_unlock = true;

}



static u16 touch_key_proc(u16 key_value)
{
    static u16 key = 0;
    static uint8_t filter_cnt = 0;
    pass_word_t pass_word;
    filter_cnt++;
    if(key != key_value)
    {
        filter_cnt = 0;
    }
    
    key = key_value;
    if(filter_cnt == 0)     //if need add button 
    {
        printf("get key \r\n");
        if(key)
        {
            u8 key_true_value = get_true_key_value(key);
            if((key_true_value != 'a') && (key_true_value != 'b'))
            {
                if(key_true_value > 0)
                {
                    pass_word.start_tick = get_tick();
                    pass_word.pass_word = key_true_value;
                    insert_one_pass_word(&pass_word);
                }
                else
                {
                    printf("key error ! \r\n");
                }
            }
            else if(key_true_value == 'b')
            {
                pass_work_proc();
            }
            
            
        }
        
        return key;
    }
    
    return  0;
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
            set_key_value( touch_key_proc(touch_key_value_raw) );
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





