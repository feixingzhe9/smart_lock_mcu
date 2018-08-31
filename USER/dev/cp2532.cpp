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

static u16 read_byte(u8 read_addr)
{
    u8 low_data = 0;
    u8 high_data = 0;

    IIC_Start();
    IIC_Send_Byte(I2C_CP2532_WRITE);  //发送写命令
    ack_flag[0] = IIC_Wait_Ack();
    IIC_Send_Byte(read_addr);   //发送地址
    ack_flag[1] = IIC_Wait_Ack();
    IIC_Stop();

    IIC_Start();
    IIC_Send_Byte(I2C_CP2532_READ); //发送读命令
    ack_flag[2] = IIC_Wait_Ack();
    high_data=IIC_Read_Byte(1);
    low_data=IIC_Read_Byte(0);

    IIC_Stop();    //产生一个停止条件
    return ((high_data<<8) + low_data);
}

static u16 write_byte(u8 addr, u16 data)
{
    u8 data_high = data >> 8;
    u8 data_low = data & 0xff;
    IIC_Start();
    IIC_Send_Byte(I2C_CP2532_WRITE);    //write command
    ack_flag[0] = IIC_Wait_Ack();

    IIC_Send_Byte(addr);//register address
    ack_flag[1] = IIC_Wait_Ack();

    IIC_Send_Byte(data_high);// write data_high first
    ack_flag[2] = IIC_Wait_Ack();
    IIC_Send_Byte(data_low);// write data_low
    ack_flag[3] = IIC_Wait_Ack();

    IIC_Stop();
    return 0;
}


#ifdef CP2532_INT_ENABLE
static u16 set_key_interrupt_trigger(void)
{
    return write_byte(I2C_CP2532_SET_KEY_INTERRUPT_MODE_ADDR, 0x0fff);
}

static u16 get_key_interrupt_trigger(void)
{
    return read_byte(I2C_CP2532_SET_KEY_INTERRUPT_MODE_ADDR);
}

static u16 set_key_interrupt(void)
{
    return write_byte(I2C_CP2532_SET_KEY_INTERRUPT_ADDR, 0x0fff);
}

static u16 get_key_interrupt(void)
{
    return read_byte(I2C_CP2532_SET_KEY_INTERRUPT_ADDR);
}
#endif


static uint16_t set_gcr_register(uint16_t value)
{
    return write_byte(I2C_CP2532_GCR_ADDR, value);
}

static uint16_t get_gcr_register(void)
{
    return read_byte(I2C_CP2532_GCR_ADDR);
}

static uint16_t set_idle_cfg_register(uint16_t value)
{
    return write_byte(I2C_CP2532_IDLE_CFG_ADDR, value);
}

static uint16_t get_idle_cfg_register(void)
{
    return read_byte(I2C_CP2532_IDLE_CFG_ADDR);
}

u16 quick_read(void)
{
    u8 low_data = 0;
    u8 high_data = 0;

    IIC_Start();
    IIC_Send_Byte(I2C_CP2532_QUICK_READ_ADDR); //发送写命令
    quick_read_ack_flag[0] = IIC_Wait_Ack();

    high_data=IIC_Read_Byte(1);
    low_data=IIC_Read_Byte(0);
    IIC_Stop(); //产生一个停止条件

    return ((high_data<<8) + low_data);
}

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
        u16 key_value_tmp = key_value;
        u8 count = 0;
        while ( key_value_tmp )
        {
            key_value_tmp &= (key_value_tmp - 1) ;
            ++count;
        }

        if(count > 1)   //count 表示 key_value 中 bit 1 的数量
        {
             return false;
        }
        return true;

    }
}


static u8 map_key_value(u16 key_value)
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

#define PASSWORD_EXIST_TIME     4000/SYSTICK_PERIOD
static void insert_one_pass_word(pass_word_t *key_info)
{
    if(pass_word_info->lenth > 0)
    {
        if(key_info->start_tick - pass_word_info->pass_word_buf[pass_word_info->lenth - 1].start_tick >= PASSWORD_EXIST_TIME)
        {
            clear_pass_word();
            memcpy( &(pass_word_info->pass_word_buf[0]), key_info, sizeof(pass_word_t));
            pass_word_info->lenth++;
            return ;
        }
    }

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


char psss_word_in_flash[PASS_WORD_LENTH] = {0};
static void pass_work_proc(void)
{
    char password[PASS_WORD_LENTH];

    if(pass_word_info->lenth == PASS_WORD_LENTH)
    {
        if(get_tick() - pass_word_info->pass_word_buf[pass_word_info->lenth - 1].start_tick >= PASSWORD_EXIST_TIME)
        {
            clear_pass_word();
            return ;
        }
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

static u16 proc_touch_key(const u16 key_value)
{
    static u16 key = 0;
    static u32 filter_cnt = 0;
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
            u8 key_true_value = map_key_value(key);
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

#ifdef CP2532_INT_ENABLE
static void cp2532_int_init(void)
{
    EXTI_InitTypeDef exit_init_structure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //----  cp2532 touch key int  ----//
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);   //使能GPIOG时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
    GPIO_Init(GPIOG, &GPIO_InitStructure);  //初始化PG11

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource11);

    exit_init_structure.EXTI_Line = EXTI_Line11;
    exit_init_structure.EXTI_Mode = EXTI_Mode_Interrupt;
    exit_init_structure.EXTI_Trigger = EXTI_Trigger_Falling;
    exit_init_structure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exit_init_structure);        //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;    //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;           //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

u16 interrupt_value = 0;

#ifdef __cplusplus
extern "C" {
#endif
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line11) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line11);
        touch_key_value_raw = read_byte(0x31);
        interrupt_value = read_byte(0x33) & 0x0fff;  // read interrupt value in register to clear cp2532 interrupt

        if(is_key_valid(touch_key_value_raw) == true)
        {
            set_key_value( proc_touch_key(touch_key_value_raw) );
        }

        if(touch_key_value_raw == 0)
        {
//          printf("get key value 0 from interrupt"); //test code
        }

        if( get_key_value() )
        {
            upload_touch_key_data( get_key_value() );
        }
    }

    if(EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line10);
        lock_status_change_start_tick = get_tick();
        is_lock_status_changed = 1;
    }

}

#ifdef __cplusplus
  }
#endif

void cp2532_init(void)
{
    u8 retry_cnt = 0;
    i2c_init();

    do
    {
        set_key_interrupt_trigger();
        delay_ms(10);
        retry_cnt++;
        if(retry_cnt >= 20)
        {
            break;
        }
    }while( 0x0fff != get_key_interrupt_trigger() );

    if(retry_cnt >= 20)
    {
        printf("fatal: set key interrupt trigger failed ! ! !");
        sys_err |= SYS_CP2532_INIT_ERR;
    }

    retry_cnt = 0;

    do
    {
        set_key_interrupt();
        delay_ms(10);
        retry_cnt++;
        if(retry_cnt >= 20)
        {
            break;
        }
    }while( 0x0fff != get_key_interrupt() );

    if(retry_cnt >= 20)
    {
        printf("fatal: set key interrupt failed ! ! !");
        sys_err |= SYS_CP2532_INIT_ERR;
    }
    retry_cnt = 0;

    do
    {
        set_gcr_register(0x0fff);
        delay_ms(10);
        retry_cnt++;
        if(retry_cnt >= 20)
        {
            break;
        }
    }while( 0x0fff != get_gcr_register() );

    if(retry_cnt >= 20)
    {
        printf("fatal: set gcr value failed ! ! !");
        sys_err |= SYS_CP2532_INIT_ERR;
    }
    retry_cnt = 0;

    do
    {
        set_idle_cfg_register(0x0132);      //change touch key backlight mode: in document/ds_cp2532_R32_ch.pdf page24 and page36
        delay_ms(10);
        retry_cnt++;
        if(retry_cnt >= 20)
        {
            break;
        }
    }while( 0x0132 != get_idle_cfg_register() );

    if(retry_cnt >= 20)
    {
        printf("fatal: set gcr value failed ! ! !");
        sys_err |= SYS_CP2532_INIT_ERR;
    }
    retry_cnt = 0;

    cp2532_int_init();

    touch_key_value_raw = read_byte(0x31);

    interrupt_value = read_byte(0x33);

//    test = get_key_interrupt();
    return;
}
#endif

#ifndef CP2532_INT_ENABLE
#define TOUCH_KEY_PERIOD    30/SYSTICK_PERIOD
void touch_key_task(void)
{
    static uint32_t start_tick = 0;
    if(get_tick() - start_tick >= TOUCH_KEY_PERIOD)
    {
        touch_key_value_raw = read_byte(0x31);
        if(is_key_valid(touch_key_value_raw))
        {
            set_key_value( proc_touch_key(touch_key_value_raw) );
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
#endif

