#include "myiic.h"
#include "delay.h"

static GPIO_InitTypeDef i2c_sda_gpio_init_struct;
static GPIO_InitTypeDef i2c_scl_gpio_init_struct;
//��ʼ��IIC
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


//����IIC��ʼ�ź�
void IIC_Start(void)
{
    //SDA_OUT();     //sda�����
    i2c_sda_out();
    //IIC_SDA=1;
    set_i2c_sda_out_1(); //set_i2c_sda_out(1);
    //IIC_SCL=1;
    set_i2c_scl_out_1();//set_i2c_scl_out(1);
    delay_us(4);
    //IIC_SDA=0;//START:when CLK is high,DATA change form high to low
    set_i2c_sda_out_0();//set_i2c_sda_out(0);
    delay_us(4);
    //IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ��������
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
}


//����IICֹͣ�ź�
void IIC_Stop(void)
{
    //SDA_OUT();//sda�����
    i2c_sda_out();
    //IIC_SCL=0;
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
    //IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    set_i2c_sda_out_0();//set_i2c_sda_out(0);
    delay_us(4);
    //IIC_SCL=1;
    set_i2c_scl_out_1();//set_i2c_scl_out(1);
    //IIC_SDA=1;//����I2C���߽����ź�
    set_i2c_sda_out_1();//set_i2c_sda_out(1);
    delay_us(4);
}


//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
    u16 ucErrTime=0;
    //SDA_IN();      //SDA����Ϊ����
    i2c_sda_in();
    //IIC_SDA=1;
    delay_us(1);
    //IIC_SCL=1;
    set_i2c_scl_out_1();//set_i2c_scl_out(1);
    delay_us(1);
    //SDA_IN();      //SDA����Ϊ����
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
    //IIC_SCL=0;//ʱ�����0
    set_i2c_scl_out_0();//set_i2c_scl_out(0);
    return 0;
}

//����ACKӦ��
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

//������ACKӦ��
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

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    //SDA_OUT();
    i2c_sda_out();
    //IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
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
        delay_us(1);   //��TEA5767��������ʱ���Ǳ����
        //IIC_SCL=1;
        set_i2c_scl_out_1();//set_i2c_scl_out(1);
        delay_us(2);
        //IIC_SCL=0;
        set_i2c_scl_out_0();//set_i2c_scl_out(0);
        delay_us(2);
    }
}

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    //SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK
    return receive;
}



