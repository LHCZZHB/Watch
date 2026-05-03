#include "./BSP/IIC/myiic.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/MPU6050/mpu6050.h"
#include "./SYSTEM/usart/usart.h"

//MPU6050 GPIO初始化
void mpu6050_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
}

//MPU6050读取一个字节数据
uint8_t mpu6050_receivebyte(uint8_t regaddress)
{
    uint8_t data;

    iic_start();
    iic_send_byte(Device_Write_Addr_mpu); // 发送 MPU6050 写地址
    iic_wait_ack();
    iic_send_byte(regaddress); // 发送寄存器地址
    iic_wait_ack();

    iic_start(); // 重新开始
    iic_send_byte(Device_Read_Addr_mpu); // 发送 MPU6050 读地址
    iic_wait_ack();
    data = iic_read_byte(0); // 读取数据后发送 NACK
    iic_stop(); // 停止 I2C 通信

    return data;
}

//MPU6050写一个字节数据
void mpu6050_sendbyte(uint8_t regaddress, uint8_t data)
{
    iic_start();
    iic_send_byte(Device_Write_Addr_mpu); // 发送 MPU6050 写地址
    iic_wait_ack();
    iic_send_byte(regaddress); // 发送寄存器地址
    iic_wait_ack();
    iic_send_byte(data); // 发送数据
    iic_wait_ack();
    iic_stop(); // 停止 I2C 通信
}

//MPU6050初始化
void mpu6050_init(void)
{
    mpu6050_gpio_init();
    iic_init(); // 初始化 I2C 接口
    uint8_t chip_id = mpu6050_receivebyte(MPU6050_WHO_AM_I);
    if(chip_id == 0x68)
    {
        printf("MPU6050 initialization successful!\r\n");
    }
    else
    {
        printf("MPU6050 initialization failed! Read Chip ID: 0x%02X\r\n", chip_id);
    }

    mpu6050_sendbyte(MPU6050_PWR_MGMT_1, 0x80); // 进行整机复位
    delay_ms(100); // 等待复位完成
    mpu6050_sendbyte(MPU6050_PWR_MGMT_1, 0x01); // 解除睡眠
	mpu6050_sendbyte(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机

    mpu6050_sendbyte(MPU6050_SMPLRT_DIV, 0x09); // 设置采样率为 1kHz / (1 + 9) = 100Hz
    mpu6050_sendbyte(MPU6050_CONFIG, 0x06); // 设置 DLPF 为 5Hz
    mpu6050_sendbyte(MPU6050_GYRO_CONFIG, 0x18); // 设置陀螺仪量程为 ±2000°/s
    mpu6050_sendbyte(MPU6050_ACCEL_CONFIG, 0x18); // 设置加速度计量程为 ±16g

    // 0x20: 设置为电平锁存模式 (直到读取状态寄存器才清除)[cite: 1]
    // 或者 0x00: 设置为 50us 脉冲模式[cite: 1]
    mpu6050_sendbyte(INT_PIN_CFG, 0x20);
    //使能数据就绪中断 (DATA_RDY_EN)[cite: 1]
    mpu6050_sendbyte(INT_ENABLE, 0x01);
    mpu6050_receivebyte(INT_STATUS);
}

//MPU6050获取加速度和陀螺仪数据
void mpu6050_getdata(int16_t *accel_data, int16_t *gyro_data)
{
    uint8_t buffer[12];

    buffer[0] = mpu6050_receivebyte(MPU6050_ACCEL_XOUT_H);		//读取加速度计X轴的高8位数据
	buffer[1] = mpu6050_receivebyte(MPU6050_ACCEL_XOUT_L);		//读取加速度计X轴的低8位数据
    accel_data[0] = (int16_t)((buffer[0] << 8) | buffer[1]); // 加速度 X

    buffer[2] = mpu6050_receivebyte(MPU6050_ACCEL_YOUT_H);		//读取加速度计Y轴的高8位数据
	buffer[3] = mpu6050_receivebyte(MPU6050_ACCEL_YOUT_L);		//读取加速度计Y轴的低8位数据
    accel_data[1] = (int16_t)((buffer[2] << 8) | buffer[3]); // 加速度 Y

    buffer[4] = mpu6050_receivebyte(MPU6050_ACCEL_ZOUT_H);		//读取加速度计Z轴的高8位数据
	buffer[5] = mpu6050_receivebyte(MPU6050_ACCEL_ZOUT_L);		//读取加速度计Z轴的低8位数据
    accel_data[2] = (int16_t)((buffer[4] << 8) | buffer[5]); // 加速度 Z
	


	buffer[6] = mpu6050_receivebyte(MPU6050_GYRO_XOUT_H);		//读取陀螺仪X轴的高8位数据
	buffer[7] = mpu6050_receivebyte(MPU6050_GYRO_XOUT_L);		//读取陀螺仪X轴的低8位数据
	gyro_data[0] = (int16_t)((buffer[6] << 8) | buffer[7]);		//数据拼接，通过输出参数返回
	
	buffer[8] = mpu6050_receivebyte(MPU6050_GYRO_YOUT_H);		//读取陀螺仪Y轴的高8位数据
	buffer[9] = mpu6050_receivebyte(MPU6050_GYRO_YOUT_L);		//读取陀螺仪Y轴的低8位数据
	gyro_data[1] = (int16_t)((buffer[8] << 8) | buffer[9]);		//数据拼接，通过输出参数返回
	
	buffer[10] = mpu6050_receivebyte(MPU6050_GYRO_ZOUT_H);		//读取陀螺仪Z轴的高8位数据
	buffer[11] = mpu6050_receivebyte(MPU6050_GYRO_ZOUT_L);		//读取陀螺仪Z轴的低8位数据
	gyro_data[2] = (int16_t)((buffer[10] << 8) | buffer[11]);	//数据拼接，通过输出参数返回
}
