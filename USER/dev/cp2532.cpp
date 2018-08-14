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

#define KEY_FIFO_SIZE   2
struct key_t
{
    u16 key;
    u32 start_tick;
};

struct key_info_t
{
    key_t key[KEY_FIFO_SIZE];
    u8 cnt;
};

key_info_t key_info_in_ram = {0};
key_info_t  * key_info = &key_info_in_ram;


cp2532_work_mode_e cp2532_work_mode = CP2532_INTERRUPT_DETECTION;

static u16 read_byte(u8 read_addr)
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

//#if (TOUCH_KEY_WORK_MODE == CP2532_INTERRUPT_DETECTION)
static u16 set_key_interrupt(void)
{    
    IIC_Start();  
    IIC_Send_Byte(I2C_CP2532_WRITE);	   //发送写命令
    ack_flag[0] = IIC_Wait_Ack();
    
    IIC_Send_Byte(I2C_CP2532_SET_KEY_INTERRUPT_ADDR);//发送地址
    ack_flag[1] = IIC_Wait_Ack();	
    
    IIC_Send_Byte(0x0f);//
    ack_flag[2] = IIC_Wait_Ack();	
    IIC_Send_Byte(0xff);//
    ack_flag[3] = IIC_Wait_Ack();	
    
    IIC_Stop();    
    
    return 0;
}
//#endif

static u16 get_key_interrupt(void)
{
    u8 low_data = 0;
    u8 high_data = 0;
    
    IIC_Start();  
    IIC_Send_Byte(I2C_CP2532_WRITE);	   //发送写命令
    ack_flag[0] = IIC_Wait_Ack();
    IIC_Send_Byte(I2C_CP2532_SET_KEY_INTERRUPT_ADDR);//发送地址
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


static u16 quick_read(void)
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

//static can_message_t touch_key_message_ram;
//static can_message_t *touch_key_message = &touch_key_message_ram;


//----  test code ----//
static void upload_touch_key_data(u16 key_value)
{
    can_message_t touch_key_test = {0};
    can_id_union id; 
    id.can_id_struct.src_mac_id = LOCK_CAN_MAC_SRC_ID;
    id.can_id_struct.source_id = CAN_SOURCE_ID_KEY_TEST_UPLOAD;
    id.can_id_struct.res = 0;
    id.can_id_struct.ack = 0;
    id.can_id_struct.func_id = 0;
    
    touch_key_test.id = id.can_id;
    touch_key_test.data[0] = 0x00;
    *(u16 *)&(touch_key_test.data[1]) = key_value;
    touch_key_test.data_len = 3;
    can.can_send( &touch_key_test );
}

static void set_key_value(u16 key_vlaue)
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

struct pass_word_info_t pass_word_info_ram = {0};
struct pass_word_info_t *pass_word_info = &pass_word_info_ram;


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


static void upload_password(const char *password)
{
    can_message_t password_msg;    
    can_id_union id; 
    id.can_id_struct.src_mac_id = LOCK_CAN_MAC_SRC_ID;
    id.can_id_struct.source_id = CAN_SOURCE_ID_PW_UPLOAD;
    id.can_id_struct.res = 0;
    id.can_id_struct.ack = 0;
    id.can_id_struct.func_id = 0;
    
    password_msg.id = id.can_id;
    password_msg.data[0] = 0x00;
    memcpy( (void *)&password_msg.data[1], password, 4 );
    password_msg.data_len = 5;
    can.can_send( &password_msg );
}

#define PASSWORD_EXIST_TIME     4000/SYSTICK_PERIOD
char psss_word_in_flash[PASS_WORD_LENTH] = {0};
static void pass_work_proc(void)
{
    char password[PASS_WORD_LENTH];
    
    for(u8 i = 0; i < pass_word_info->lenth; i++)
    {
        if(get_tick() - pass_word_info->pass_word_buf[i].start_tick >= PASSWORD_EXIST_TIME)
        {
            shift_letf_pass_word();
        }
    }   
    
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
    
    upload_password(password);  //upload password whether it is super password
    
    for(u8 i = 0; i < PASS_WORD_LENTH; i++)
    {
        if(password[i] != psss_word_in_flash[i])
        {
            return ;
        }            
    }
       
    // ----   get right password here  ----//
    printf("get right password");
    clear_pass_word();
    start_to_unlock_all();

}

static u16 touch_key_proc(const u16 key_value)
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
    if(filter_cnt == 0)     //如果需要按键消抖 filter_cnt 应该大于 0
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



//#if (TOUCH_KEY_WORK_MODE == CP2532_INTERRUPT_DETECTION)
static u16 touch_key_proc_int(const u16 key_value)
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
    if(filter_cnt == 0)     //如果需要按键消抖 filter_cnt 应该大于 0
    {
//        printf("get key \r\n");
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
//                    printf("key error ! \r\n");
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
//#endif


//#if (TOUCH_KEY_WORK_MODE == CP2532_INTERRUPT_DETECTION)
static void cp2532_int_init(void)
{
    EXTI_InitTypeDef exit_init_structure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
     //----  cp2532 touch key int  ----//
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	//使能GPIOG时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
    GPIO_Init(GPIOG, &GPIO_InitStructure);  //初始化PG11
    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource11);

  	exit_init_structure.EXTI_Line=EXTI_Line11;	//
  	exit_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	exit_init_structure.EXTI_Trigger = EXTI_Trigger_Falling;
  	exit_init_structure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&exit_init_structure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
    
  
}
//#endif


//#if (TOUCH_KEY_WORK_MODE == CP2532_INTERRUPT_DETECTION)



static void key_info_init(void)
{
    key_info->cnt = 0;
    for(u8 i = 0; i < KEY_FIFO_SIZE; i++)
    {
        key_info->key[i].key = 0;
        key_info->key[i].start_tick = 0;
    }  
}

static void shift_left_key(void)
{
    if(key_info->cnt > 0)
    {
        for(u8 i = 1; i <  key_info->cnt; i++)
        {
            memcpy( &(key_info->key[i - 1]), &(key_info->key[i]), sizeof(key_t));
        }
        key_info->cnt--;
    }    
}

static void insert_one_key(u16 key)
{
    if(key_info->cnt < KEY_FIFO_SIZE)
    {
        key_info->key[key_info->cnt].key = key;
        key_info->key[key_info->cnt].start_tick = get_tick();
        key_info->cnt++;
    }
    else
    {
        shift_left_key();
        key_info->key[key_info->cnt].key = key;
        key_info->key[key_info->cnt].start_tick = get_tick();
        key_info->cnt++;      
    }
}

static void clear_key(void)
{
    key_info_init();
}

#define KEY_FILTER_VALID_PERIOD     10/SYSTICK_PERIOD
static u16 key_filter(void)
{
//    u32 key_sum;
    if(key_info->cnt == KEY_FIFO_SIZE)
    {
        for(u8 i = 1; i < KEY_FIFO_SIZE; i++)
        {
            if(key_info->key[i].key ^ key_info->key[i - 1].key)
            {
                key_info->cnt = 0;
                return 0;
            }
        }
        if(get_tick() - key_info->key[KEY_FIFO_SIZE - 1].start_tick >= KEY_FILTER_VALID_PERIOD)
        {
            u16 key = key_info->key[0].key;
            key_info->cnt = 0;
            return key;
        }
    }
    
    return 0;
}

u16 get_key(void)
{
    u16 key = 0;
    __disable_irq();    //停止响应中断 ：仅仅是停止响应中断，中断还是会进来，在 __enable_irq(); 后 立即响应中断
    key = key_filter();    
    __enable_irq();     //恢复响应中断
    return key;
}
//#endif

u16 interrupt_value = 0;

void cp2532_init(void)
{
//    u16 test = 0;
    u8 retry_cnt = 0;
    i2c_init();
    
//#if (TOUCH_KEY_WORK_MODE == CP2532_INTERRUPT_DETECTION)   
    
    do
    {
        set_key_interrupt();
        delay_ms(10);
        retry_cnt++;
    }while((0x0fff != get_key_interrupt()) || (retry_cnt >= 20));
    
    if(retry_cnt >= 10)
    {
//        cp2532_work_mode = CP2532_POLLING_DETECTION;
        printf("fatal: set key interrupt failed ! ! !");
    }
    else
    {
//        cp2532_work_mode = CP2532_INTERRUPT_DETECTION;
    }
    
    
    key_info_init();
    cp2532_int_init();
    
    touch_key_value_raw = read_byte(0x31);
    
    interrupt_value = read_byte(0x33);
//#endif
//    test = get_key_interrupt();
    return;
}





#ifdef __cplusplus
extern "C" {
#endif  

void EXTI15_10_IRQHandler(void)
{
    EXTI_ClearITPendingBit(EXTI_Line11);

//#if (TOUCH_KEY_WORK_MODE == CP2532_INTERRUPT_DETECTION)
    
//    if(cp2532_work_mode == CP2532_POLLING_DETECTION)
//    {
//        return ;
//    }
    
//    touch_key_value_raw = read_byte(0x31);
//    interrupt_value = read_byte(0x33);  // read interrupt value in register to clear cp2532 interrupt
      touch_key_value_raw = read_byte(0x33) & 0x0fff;
    if(touch_key_value_raw > 0)
    {
        if(is_key_valid(touch_key_value_raw) == true)
        {
            
            insert_one_key(touch_key_value_raw);
    //        set_key_value( touch_key_proc_int(touch_key_value_raw) );
        }
        else
        {
    //        printf("key is invalid ! \r\n");
        }
    }
    
    
  
//    
//    if( get_key_value() )
//    {
//        upload_touch_key_data( get_key_value() );
//    }
    
//#endif
    
    
    interrupt_value = read_byte(0x33);  // read interrupt value in register to clear cp2532 interrupt

}

#ifdef __cplusplus
  }
#endif



#define TOUCH_KEY_PERIOD    30/SYSTICK_PERIOD
void touch_key_task(void)
{
    
//    if(cp2532_work_mode == CP2532_INTERRUPT_DETECTION)
//    {
//        return ;
//    }
//    

//#if (TOUCH_KEY_WORK_MODE == CP2532_POLLING_DETECTION)
//    static uint32_t start_tick = 0;
//    if(get_tick() - start_tick >= TOUCH_KEY_PERIOD)
//    {
//        touch_key_value_raw = read_byte(0x31);
//        
////        interrupt_value = read_byte(0x33);
//        
//        if(is_key_valid(touch_key_value_raw) == true)
//        {
//            set_key_value( touch_key_proc(touch_key_value_raw) );
//        }
//        else
//        {
//            printf("key is invalid ! \r\n");
//        }
//        
//        if( get_key_value() )
//        {
//            upload_touch_key_data( get_key_value() );
//        }
//        
//        start_tick = get_tick();
//    } 
//#endif
    
    u16 key = get_key();
    set_key_value( touch_key_proc(key) );
      
}

