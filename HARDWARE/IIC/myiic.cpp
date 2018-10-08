#include "myiic.h"
#include "delay.h"

static GPIO_InitTypeDef i2c_sda_gpio_init_struct;
static GPIO_InitTypeDef i2c_scl_gpio_init_struct;
//初始化IIC
void i2c_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_I2C_PORT, ENABLE);

    i2c_sda_gpio_init_struct.GPIO_Pin = I2C_SDA_PIN;
    i2c_sda_gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    i2c_sda_gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_SDA_PORT, &i2c_sda_gpio_init_struct);

    i2c_scl_gpio_init_struct.GPIO_Pin = I2C_SCL_PIN;
    i2c_scl_gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    i2c_scl_gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_SCL_PORT, &i2c_scl_gpio_init_struct);

    GPIO_SetBits(I2C_SDA_PORT, I2C_SDA_PIN);
    GPIO_SetBits(I2C_SCL_PORT, I2C_SCL_PIN);
}

inline void i2c_sda_in(void)
{
//    i2c_sda_gpio_init_struct.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_Init(I2C_SDA_PORT, &i2c_sda_gpio_init_struct);
    SDA_IN();
}

inline void i2c_sda_out(void)
{
//    i2c_sda_gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(I2C_SDA_PORT, &i2c_sda_gpio_init_struct);
    SDA_OUT();
}

uint8_t get_i2c_sda_in(void)
{
    return GPIO_ReadInputDataBit(I2C_SDA_PORT, I2C_SDA_PIN);
}


inline void set_i2c_sda_out_1(void)
{
    //GPIO_SetBits(I2C_SDA_PORT, I2C_SDA_PIN);
    I2C_SDA_PORT->BSRR = I2C_SDA_PIN;
}

inline void set_i2c_sda_out_0(void)
{
    //GPIO_ResetBits(I2C_SDA_PORT, I2C_SDA_PIN);
    I2C_SDA_PORT->BRR = I2C_SDA_PIN;
}


inline void set_i2c_scl_out_1(void)
{
    //GPIO_SetBits(I2C_SCL_PORT, I2C_SCL_PIN);
    I2C_SCL_PORT->BSRR = I2C_SCL_PIN;
}

inline void set_i2c_scl_out_0(void)
{
    //GPIO_ResetBits(I2C_SCL_PORT, I2C_SCL_PIN);
    I2C_SCL_PORT->BRR = I2C_SCL_PIN;
}


//产生IIC起始信号
void IIC_Start(void)
{
    //SDA_OUT();     //sda线输出
    i2c_sda_out();
    //IIC_SDA=1;
    set_i2c_sda_out_1(); //set_i2c_sda_out(1);
    //IIC_SCL=1;
    set_i2c_scl_out_1();//set_i2c_scl_out(1);
    delay_us(4);
    //IIC_SDA=0;//START:when CLK is high,DATA change form high to low
    set_i2c_sda_out_0();//set_i2c_sda_out(0);
    delay_us(4);
    //IIC_SCL=0;//钳住I2C总线，准备发送或接收数据
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
}


//产生IIC停止信号
void IIC_Stop(void)
{
    //SDA_OUT();//sda线输出
    i2c_sda_out();
    //IIC_SCL=0;
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
    //IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    set_i2c_sda_out_0();//set_i2c_sda_out(0);
    delay_us(4);
    //IIC_SCL=1;
    set_i2c_scl_out_1();//set_i2c_scl_out(1);
    //IIC_SDA=1;//发送I2C总线结束信号
    set_i2c_sda_out_1();//set_i2c_sda_out(1);
    delay_us(4);
}


//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
    u16 ucErrTime=0;
    //SDA_IN();      //SDA设置为输入
    i2c_sda_in();
    //IIC_SDA=1;
    delay_us(1);
    //IIC_SCL=1;
    set_i2c_scl_out_1();//set_i2c_scl_out(1);
    delay_us(1);
    //SDA_IN();      //SDA设置为输入
    while(READ_SDA)
    //while(get_i2c_sda_in())
    {
        ucErrTime++;
        if(ucErrTime>250)
            {
            IIC_Stop();
            return 1;
        }
    }
    //IIC_SCL=0;//时钟输出0
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
    return 0;
}

//产生ACK应答
void IIC_Ack(void)
{
    //IIC_SCL=0;
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
    //SDA_OUT();
    i2c_sda_out();
    //IIC_SDA=0;
    set_i2c_sda_out_0();//set_i2c_sda_out(0);
    delay_us(2);
    //IIC_SCL=1;
    set_i2c_scl_out_1();//set_i2c_scl_out(1);
    delay_us(2);
    //IIC_SCL=0;
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
}

//不产生ACK应答
void IIC_NAck(void)
{
    //IIC_SCL=0;
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
    //SDA_OUT();
    i2c_sda_out();
    //IIC_SDA=1;
    set_i2c_sda_out_1();//set_i2c_sda_out(1);
    delay_us(2);
    //IIC_SCL=1;
    set_i2c_scl_out_1();//set_i2c_scl_out(1);
    delay_us(2);
    //IIC_SCL=0;
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    //SDA_OUT();
    i2c_sda_out();
    //IIC_SCL=0;//拉低时钟开始数据传输
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
    delay_us(2);
    for(t=0;t<8;t++)
    {
        //IIC_SDA=(txd&0x80)>>7;
        if((txd&0x80)>>7)
        //IIC_SDA=1;
            set_i2c_sda_out_1();//set_i2c_sda_out(1);
        else
        //IIC_SDA=0;
            set_i2c_sda_out_0();//set_i2c_sda_out(0);
        txd<<=1;
        delay_us(1);   //对TEA5767这三个延时都是必须的
        //IIC_SCL=1;
        set_i2c_scl_out_1();//set_i2c_scl_out(1);
        delay_us(2);
        //IIC_SCL=0;
        set_i2c_scl_out_0();//set_i2c_scl_out(0);
        delay_us(2);
    }
}

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    //SDA_IN();//SDA设置为输入
    i2c_sda_in();
    for(i=0;i<8;i++ )
    {
        //IIC_SCL=0;
        set_i2c_scl_out_0();//set_i2c_scl_out(0);
        delay_us(2);
        //IIC_SCL=1;
        set_i2c_scl_out_1();//set_i2c_scl_out(1);
        receive<<=1;
        if(READ_SDA)
        //if(get_i2c_sda_in())
        receive++;
        delay_us(1);
    }
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK
    return receive;
}



