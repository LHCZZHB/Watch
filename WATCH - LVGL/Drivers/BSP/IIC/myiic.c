#include "./BSP/IIC/myiic.h"
#include "./SYSTEM/delay/delay.h"

void iic_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE(); // 使能 GPIOB 时钟

    // 配置 SCL 和 SDA
    gpio_init_struct.Pin = GPIO_PIN_8; // SCL -> PB8
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;      // 推挽输出
    gpio_init_struct.Pull = GPIO_PULLUP;              // 上拉
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;    // 高速
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);          // 初始化 GPIOB

    gpio_init_struct.Pin = GPIO_PIN_9; // SDA -> PB9
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;      // 开漏输出
    HAL_GPIO_Init(GPIOB, &gpio_init_struct);          // 初始化 GPIOB

    // 默认 SCL 和 SDA 拉高
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
}

void iic_start(void)
{
    // I2C 起始条件：SCL 高电平时，SDA 由高变低
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET); // SDA 高
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); // SCL 高
    delay_us(4); // 延时 4 微秒
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET); // SDA 低，产生起始信号
    delay_us(4); // 延时 4 微秒
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); // SCL 低
}

void iic_stop(void)
{
    // I2C 停止条件：SCL 高电平时，SDA 由低变高
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET); // SDA 低
    delay_us(4); // 延时 4 微秒
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); // SCL 高
    delay_us(4); // 延时 4 微秒
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET); // SDA 高，产生停止信号
    delay_us(4); // 延时 4 微秒
}   

uint8_t iic_wait_ack(void)
{
    // 等待从机 ACK
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET); // 释放 SDA
    delay_us(4); // 延时 4 微秒
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); // SCL 拉高
    delay_us(4); // 延时 4 微秒
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)) // 读取 SDA 状态
    {
        iic_stop(); // 未收到 ACK，退出
        return 1; // 1 表示未应答
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); // SCL 拉低
    delay_us(4); // 延时 4 微秒
    return 0; // 0 表示收到 ACK
}

void iic_ack(void)
{
    // 发送 ACK
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); // SCL 拉低
    delay_us(4); // 延时 4 微秒
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET); // SDA 拉低，表示 ACK
    delay_us(4); // 延时 4 微秒
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); // SCL 拉高，ACK 生效
    delay_us(4); // 延时 4 微秒
}

void iic_nack(void)
{
    // 发送 NACK
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); // SCL 拉低
    delay_us(4); // 延时 4 微秒
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET); // SDA 拉高，表示 NACK
    delay_us(4); // 延时 4 微秒
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); // SCL 拉高，NACK 生效
    delay_us(4); // 延时 4 微秒
}

void iic_send_byte(uint8_t byte)
{
    // 发送 1 字节
    for(uint8_t i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, (byte & 0x80) >> 7); // 输出最高位
        delay_us(4); // 延时 4 微秒
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); // SCL 拉高
        delay_us(4); // 延时 4 微秒
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); // SCL 拉低
        byte <<= 1; // 左移一位
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET); // 释放 SDA，等待 ACK
}

uint8_t iic_read_byte(uint8_t ack)
{
    // 读取 1 字节
    uint8_t receive = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        receive <<= 1; // 先左移一位
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); // SCL 拉高
        delay_us(4); // 延时 4 微秒
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)) // 读取 SDA 状态
        {
            receive++; // SDA 为高则置 1
        }
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); // SCL 拉低，准备下一位
        delay_us(4); // 延时 4 微秒
    }
    if(!ack) iic_nack(); // 发送 NACK
    else iic_ack(); // 发送 ACK
   
    return receive; // 返回读取结果
}


// 向指定地址写入 1 字节
uint8_t iic_write_one_byte(uint8_t addr_7bit, uint8_t reg, uint8_t data)
{
    iic_start(); // 发送起始信号
    iic_send_byte(addr_7bit << 1); // 7 位地址 + 写位
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 1; // 1 表示失败
    }
    iic_send_byte(reg); // 写寄存器地址
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 1; // 1 表示失败
    }
    iic_send_byte(data); // 写数据
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 1; // 1 表示失败
    }
    iic_stop(); // 发送停止信号
    return 0; // 0 表示成功
}

// 从指定地址读取 1 字节
uint8_t iic_read_one_byte(uint8_t addr_7bit, uint8_t reg)
{
    iic_start(); // 发送起始信号
    iic_send_byte(addr_7bit << 1); // 7 位地址 + 写位
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 0xFF; // 0xFF 表示失败
    }
    iic_send_byte(reg); // 写寄存器地址
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 0xFF; // 0xFF 表示失败
    }
    iic_start(); // 重新发送起始信号
    iic_send_byte((addr_7bit << 1) | 0x01); // 7 位地址 + 读位
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 0xFF; // 0xFF 表示失败
    }
    uint8_t data = iic_read_byte(0); // 读取数据，最后发送 NACK
    iic_stop(); // 发送停止信号
    return data; // 返回读取结果
}

// 向指定地址连续写入多个字节
uint8_t iic_write_multi_byte(uint8_t addr_7bit, uint8_t reg, uint8_t length, uint8_t *data)
{
    iic_start(); // 发送起始信号
    iic_send_byte(addr_7bit << 1); // 7 位地址 + 写位
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 1; // 1 表示失败
    }
    iic_send_byte(reg); // 写寄存器地址
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 1; // 1 表示失败
    }
    for(uint8_t i = 0; i < length; i++)
    {
        iic_send_byte(data[i]); // 逐字节写入
        if(iic_wait_ack()) // 等待应答
        {
            iic_stop(); // 未应答，停止
            return 1; // 1 表示失败
        }
    }
    iic_stop(); // 发送停止信号
    return 0; // 0 表示成功
}

// 从指定地址连续读取多个字节
uint8_t iic_read_multi_byte(uint8_t addr_7bit, uint8_t reg, uint8_t length, uint8_t *data)
{
    iic_start(); // 发送起始信号
    iic_send_byte(addr_7bit << 1); // 7 位地址 + 写位
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 1; // 1 表示失败
    }
    iic_send_byte(reg); // 写寄存器地址
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 1; // 1 表示失败
    }
    iic_start(); // 重新发送起始信号
    iic_send_byte((addr_7bit << 1) | 0x01); // 7 位地址 + 读位
    if(iic_wait_ack()) // 等待应答
    {
        iic_stop(); // 未应答，停止
        return 1; // 1 表示失败
    }
    for(uint8_t i = 0; i < length; i++)
    {
        data[i] = iic_read_byte(i == (length - 1) ? 0 : 1); // 最后一个字节发送 NACK
    }
    iic_stop(); // 发送停止信号
    return 0; // 0 表示成功
}
/**
 * @brief 扫描指定 I2C 设备地址是否存在
 * @param addr_7bit: 7 位设备地址
 * @return 0: 存在, 1: 不存在
 */
uint8_t iic_device_scan(uint8_t addr_7bit) 
{
    uint8_t res;
    iic_start();                  // 发送起始信号
    iic_send_byte(addr_7bit << 1); // 发送写地址
    res = iic_wait_ack();         // 读取应答状态
    iic_stop();                   // 发送停止信号
    return res;                   // 返回扫描结果
}