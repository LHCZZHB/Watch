#include "./BSP/IIC/myiic.h"
#include "./SYSTEM/delay/delay.h"

void iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE(); // 使能 GPIOB 时钟

    // 配置 SCL 和 SDA
    gpio_init_struct.Pin = GPIO_PIN_8; // SCL -> PB8
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;      // 开漏输出
    gpio_init_struct.Pull = GPIO_PULLUP;              // 上拉
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;    // 高速
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);          // 初始化 GPIOB

    gpio_init_struct.Pin = GPIO_PIN_9; // SDA -> PB9
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;      // 开漏输出
    gpio_init_struct.Pull = GPIO_PULLUP;              // 上拉
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);          // 初始化 GPIOB

    // 默认 SCL 和 SDA 拉高
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
}

void iic_delay(void)
{
    delay_us(5); // I2C 时序要求，延时 5 微秒
}


void iic_start(void)
{
    // I2C 起始条件：SCL 高电平时，SDA 由高变低
    SDA_H();
    SCL_H();
    iic_delay(); // 延时 5 微秒

    SDA_L(); // SDA 由高变低，产生起始信号
    iic_delay(); // 延时 5 微秒

    SCL_L(); // SCL 低
    iic_delay(); // 延时 5 微秒
}

void iic_stop(void)
{
    // I2C 停止条件：SCL 高电平时，SDA 由低变高
    SCL_L(); // SCL 低
    SDA_L(); // SDA 低
    iic_delay(); // 延时 5 微秒

    SCL_H(); // SCL 高
    iic_delay(); // 延时 5 微秒

    SDA_H(); // SDA 高，产生停止信号
    iic_delay(); // 延时 5 微秒
}   

uint8_t iic_wait_ack(void)
{
    // 等待从机 ACK
    SDA_H(); // 释放 SDA，准备接收 ACK
    iic_delay(); // 延时 5 微秒
    SCL_H(); // SCL 拉高
    iic_delay(); // 延时 5 微秒
    if(SDA_READ()) // 读取 SDA 状态
    {
        iic_stop(); // 未收到 ACK，退出
        return 1; // 1 表示未应答
    }
    SCL_L(); // SCL 低电平表示结束ACK检查
    iic_delay(); // 延时 5 微秒
    return 0; // 0 表示收到 ACK
}

void iic_ack(void)
{
    // 发送 ACK
    SCL_L(); // SCL 拉低
    SDA_L(); // SDA 拉低，表示 ACK
    iic_delay(); // 延时 5 微秒
    SCL_H(); // SCL 拉高，ACK 生效
    iic_delay(); // 延时 5 微秒
    SCL_L(); // SCL 拉低，准备下一位
    iic_delay(); // 延时 5 微秒
}

void iic_nack(void)
{
    // 发送 NACK
    SCL_L(); // SCL 拉低
    SDA_H(); // SDA 拉高，表示 NACK
    iic_delay(); // 延时 5 微秒
    SCL_H(); // SCL 拉高，NACK 生效
    iic_delay(); // 延时 5 微秒
    SCL_L(); // SCL 拉低，准备下一位
    iic_delay(); // 延时 5 微秒
}

void iic_send_byte(uint8_t byte)
{
    SCL_L(); // SCL 拉低，准备输出数据
    iic_delay(); // 延时 5 微秒
    // 发送 1 字节
    for(uint8_t i = 0; i < 8; i++)
    {
        if((byte & 0x80) > 0) // 当前最高位为1
        {
            SDA_H(); // SDA 拉高
        }
        else
        {
            SDA_L(); // SDA 拉低
        }
        byte <<= 1; // 左移一位，准备发送下一位
        SCL_H(); // SCL 拉高，数据生效
        iic_delay(); // 延时 5 微秒
        SCL_L(); // SCL 拉低，准备下一位
        iic_delay(); // 延时 5 微秒
    }
    SDA_H(); // 发送完毕后释放 SDA
}

uint8_t iic_read_byte(uint8_t ack)
{
    // 读取 1 字节
    uint8_t receive = 0;
    SDA_H(); // 释放 SDA，准备接收数据
    iic_delay(); // 延时 5 微秒
    for(uint8_t i = 0; i < 8; i++)
    {
        SCL_L(); // SCL 拉低，准备接收数据
        iic_delay(); // 延时 5 微秒
        SCL_H(); // SCL 拉高
        iic_delay(); // 延时 5 微秒
        receive <<= 1; // 先左移一位

        if(SDA_READ()) // 读取 SDA 状态
        {
            receive++; // SDA 为高则置 1
        }
        SCL_L(); // SCL 拉低，准备下一位
        iic_delay(); // 延时 5 微秒
    }   
    if(!ack) iic_nack(); // 最后一个字节发送 NACK
    else iic_ack();      // 继续接收时发送 ACK
    return receive; // 返回读取结果
}
