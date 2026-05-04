#include "./BSP/IIC/myiic.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/MPU6050/mpu6050.h"
#include "./SYSTEM/usart/usart.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include <math.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define q30                    1073741824.0f    //q30格式，long转float时的除数

extern SemaphoreHandle_t MutexSemaphore;
//陀螺仪方向设置
//static signed char gyro_orientation[9] = { 1,  0,  0,
//                                           0,  1,  0,
//                                           0,  0,  1};
float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
unsigned long sensor_timestamp;
short gyro[3], accel[3], sensors;
unsigned char more;
long quat[4];

//MPU6050 GPIO初始化
// void mpu6050_gpio_init(void)
// {
//     GPIO_InitTypeDef GPIO_InitStruct = {0};
// }

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

//MPU6050连续读取多个字节数据
uint8_t mpu6050_receivemultibytes(uint8_t dev_adder, uint8_t regaddress, uint8_t *data, uint8_t length)
{
    //dev_adder参数为了适配DMP库的函数接口，不调用

    iic_start();
    iic_send_byte(Device_Write_Addr_mpu); // 发送 MPU6050 写地址
    if(iic_wait_ack())
    {
        iic_stop();
        return 1;
    }
    iic_send_byte(regaddress); // 发送寄存器地址
    if(iic_wait_ack())
    {
        iic_stop();
        return 1;
    }

    iic_start(); // 重新开始
    iic_send_byte(Device_Read_Addr_mpu); // 发送 MPU6050 读地址
    if(iic_wait_ack())
    {
        iic_stop();
        return 1;
    }

    for(uint8_t i = 0; i < length; i++)
    {
        data[i] = iic_read_byte(i < length - 1); // 读取数据，最后一个字节发送 NACK
    }
    
    iic_stop(); // 停止 I2C 通信
    return 0; // 成功读取
}

//MPU6050连续写入多个字节数据
uint8_t mpu6050_sendmultibytes(uint8_t dev_adder, uint8_t regaddress, uint8_t *data, uint8_t length)
{
    //dev_adder参数为了适配DMP库的函数接口，不调用
    iic_start();
    iic_send_byte(Device_Write_Addr_mpu); // 发送 MPU6050 写地址
    if(iic_wait_ack())
    {
        iic_stop();
        return 1;
    }
    iic_send_byte(regaddress); // 发送寄存器地址
    if(iic_wait_ack())
    {
        iic_stop();
        return 1;
    }

    for(uint8_t i = 0; i < length; i++)
    {
        iic_send_byte(data[i]); // 发送数据
        if(iic_wait_ack())
        {
            iic_stop();
            return 1;
        }
    }
    
    iic_stop(); // 停止 I2C 通信
    return 0; // 成功写入
}

//MPU6050初始化
// void mpu6050_init(void)
// {
//     mpu6050_gpio_init();
//     iic_init(); // 初始化 I2C 接口
//     uint8_t chip_id = mpu6050_receivebyte(MPU6050_WHO_AM_I);
//     if(chip_id == 0x68)
//     {
//         printf("MPU6050 initialization successful!\r\n");
//     }
//     else
//     {
//         printf("MPU6050 initialization failed! Read Chip ID: 0x%02X\r\n", chip_id);
//     }

//     mpu6050_sendbyte(MPU6050_PWR_MGMT_1, 0x80); // 进行整机复位
//     delay_ms(100); // 等待复位完成
//     mpu6050_sendbyte(MPU6050_PWR_MGMT_1, 0x01); // 解除睡眠
// 	mpu6050_sendbyte(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机

//     mpu6050_sendbyte(MPU6050_SMPLRT_DIV, 0x31); // 设置采样率为 1kHz / (1 + 49) = 20Hz
//     mpu6050_sendbyte(MPU6050_CONFIG, 0x01); // 设置 DLPF 为 1Hz
//     mpu6050_sendbyte(MPU6050_GYRO_CONFIG, 0x08); // 设置陀螺仪量程为 ±500°/s
//     mpu6050_sendbyte(MPU6050_ACCEL_CONFIG, 0x00); // 设置加速度计量程为 ±2g

//     // 0x20: 设置为电平锁存模式 (直到读取状态寄存器才清除)[cite: 1]
//     // 或者 0x00: 设置为 50us 脉冲模式[cite: 1]
//     mpu6050_sendbyte(INT_PIN_CFG, 0x20);
//     //使能数据就绪中断 (DATA_RDY_EN)[cite: 1]
//     mpu6050_sendbyte(INT_ENABLE, 0x01);
//     mpu6050_receivebyte(INT_STATUS);
// }

//DMP初始化与配置
void mpu6050_dmp_init(void)
{
    uint8_t result = 0;
    uint8_t retry = 0;
    iic_init(); // 初始化 I2C 接口
    
    result = mpu_init();
    // DMP 初始化
    if (!result)
    {
        printf("DMP initialization successful!\r\n");

        //设置所需要的传感器
        result = mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
        if(!result)
        {
            printf("Sensors set successfully!\r\n");
        }
        else
        {
            printf("Failed to set sensors!\r\n");
        }

        //设置FIFO
        result = mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
        if(!result)        
        {
            printf("FIFO configured successfully!\r\n");
        }
        else
        {
            printf("Failed to configure FIFO!\r\n");
        }

        //设置采样率
        result = mpu_set_sample_rate(100);
        if(!result)        
        {
            printf("Sample rate set successfully!\r\n");
        }
        else
        {
            printf("Failed to set sample rate!\r\n");
        }   

        //加载 DMP 固件，允许重试以规避偶发 I2C 毛刺
        for(retry = 0; retry < 3; retry++)
        {
            result = dmp_load_motion_driver_firmware();
            if(!result)
            {
                printf("DMP firmware loaded successfully!\r\n");
                break;
            }
            delay_ms(10);
        }
        if(result)
        {
            printf("Failed to load DMP firmware!\r\n");
            while (1);
        }

        //设置陀螺仪方向
        // result = dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
        // if(!result)        
        // {
        //     printf("DMP orientation set successfully!\r\n");
        // }
        // else
        // {
        //     printf("Failed to set DMP orientation!\r\n");
        // }

        //设置DMP功能
        result = dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | 
            DMP_FEATURE_SEND_RAW_ACCEL | 
            DMP_FEATURE_SEND_RAW_GYRO |
            DMP_FEATURE_PEDOMETER
        );
        if(!result)        
        {
            printf("DMP features enabled successfully!\r\n");
        }
        else
        {
            printf("Failed to enable DMP features!\r\n");
        }   

        //设置DMP输出速率
        result = dmp_set_fifo_rate(100);
        if(!result)        
        {
            printf("DMP FIFO rate set successfully!\r\n");
        }
        else
        {
            printf("Failed to set DMP FIFO rate!\r\n");
        }

        //自检
        // result = run_self_test();
        // if(!result)        
        // {
        //     printf("DMP self-test passed!\r\n");
        // }
        // else
        // {
        //     printf("DMP self-test failed!\r\n");
        // }

        //使能DMP
        result = dmp_enable_gyro_cal(1);
        if(!result)        
        {
            printf("DMP gyro calibration enabled successfully!\r\n");
        }
        else
        {
            printf("Failed to enable DMP gyro calibration!\r\n");
        }

        // 所有 DMP 参数配置完成后，再真正打开 DMP
        for(retry = 0; retry < 3; retry++)
        {
            result = mpu_set_dmp_state(1);
            if(!result)
            {
                printf("DMP state enabled successfully!\r\n");
                break;
            }
            delay_ms(10);
        }
        if(result)
        {
            printf("Failed to enable DMP state!\r\n");
            while (1);
        }
    }
    else
    {
        printf("DMP initialization failed!\r\n");
        while (1);
    }

    // 配置 DMP
}
//MPU6050获取加速度和陀螺仪数据
// void mpu6050_getdata(int16_t *accel_data, int16_t *gyro_data)
// {
//     uint8_t buffer[14];
//     uint8_t status;

    // buffer[0] = mpu6050_receivebyte(MPU6050_ACCEL_XOUT_H);		//读取加速度计X轴的高8位数据
	// buffer[1] = mpu6050_receivebyte(MPU6050_ACCEL_XOUT_L);		//读取加速度计X轴的低8位数据
    // accel_data[0] = (int16_t)((buffer[0] << 8) | buffer[1]); // 加速度 X

    // buffer[2] = mpu6050_receivebyte(MPU6050_ACCEL_YOUT_H);		//读取加速度计Y轴的高8位数据
	// buffer[3] = mpu6050_receivebyte(MPU6050_ACCEL_YOUT_L);		//读取加速度计Y轴的低8位数据
    // accel_data[1] = (int16_t)((buffer[2] << 8) | buffer[3]); // 加速度 Y

    // buffer[4] = mpu6050_receivebyte(MPU6050_ACCEL_ZOUT_H);		//读取加速度计Z轴的高8位数据
	// buffer[5] = mpu6050_receivebyte(MPU6050_ACCEL_ZOUT_L);		//读取加速度计Z轴的低8位数据
    // accel_data[2] = (int16_t)((buffer[4] << 8) | buffer[5]); // 加速度 Z
	


	// buffer[6] = mpu6050_receivebyte(MPU6050_GYRO_XOUT_H);		//读取陀螺仪X轴的高8位数据
	// buffer[7] = mpu6050_receivebyte(MPU6050_GYRO_XOUT_L);		//读取陀螺仪X轴的低8位数据
	// gyro_data[0] = (int16_t)((buffer[6] << 8) | buffer[7]);		//数据拼接，通过输出参数返回
	
	// buffer[8] = mpu6050_receivebyte(MPU6050_GYRO_YOUT_H);		//读取陀螺仪Y轴的高8位数据
	// buffer[9] = mpu6050_receivebyte(MPU6050_GYRO_YOUT_L);		//读取陀螺仪Y轴的低8位数据
	// gyro_data[1] = (int16_t)((buffer[8] << 8) | buffer[9]);		//数据拼接，通过输出参数返回
	
	// buffer[10] = mpu6050_receivebyte(MPU6050_GYRO_ZOUT_H);		//读取陀螺仪Z轴的高8位数据
	// buffer[11] = mpu6050_receivebyte(MPU6050_GYRO_ZOUT_L);		//读取陀螺仪Z轴的低8位数据
	// gyro_data[2] = (int16_t)((buffer[10] << 8) | buffer[11]);	//数据拼接，通过输出参数返回

//     status = mpu6050_receivemultibytes(0, MPU6050_ACCEL_XOUT_H, buffer, 14);
//     accel_data[0] = (int16_t)((buffer[0] << 8) | buffer[1]); // 加速度 X
//     accel_data[1] = (int16_t)((buffer[2] << 8) | buffer[3]); // 加速度 Y
//     accel_data[2] = (int16_t)((buffer[4] << 8) | buffer[5]); // 加速度 Z
//     gyro_data[0] = (int16_t)((buffer[8] << 8) | buffer[9]);		//陀螺仪 X
//     gyro_data[1] = (int16_t)((buffer[10] << 8) | buffer[11]);		//陀螺仪 Y
//     gyro_data[2] = (int16_t)((buffer[12] << 8) | buffer[13]);	//陀螺仪 Z
// }

//获取DMP数据并计算欧拉角
uint8_t mpu6050_readDMP(float *pitch, float *roll, float *yaw)
{
    int res = dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);
    if(res != 0)
    {
        printf("DMP Read Error: %d\r\n", res);
        return (uint8_t)res; // 读取失败
    }
    if (sensors & INV_WXYZ_QUAT) 
    {
        // 计算欧拉角
        q0 = quat[0] / q30; // 四元数归一化
        q1 = quat[1] / q30;
        q2 = quat[2] / q30;
        q3 = quat[3] / q30;

        *pitch = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3; 	
        *roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3;
        *yaw   = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * 57.3;
        return 0;
    }
    else 
    {
        return 2; // 返回错误代码
    }
}

/**
 * @brief  获取DMP计步器数据
 * @param  steps : 存储步数的变量指针
 * @param  walk_time : 存储行走时间（毫秒）的变量指针
 * @retval 0:成功, 其他:失败
 */
uint8_t mpu6050_get_steps(unsigned long *steps, unsigned long *walk_time)
{
    // 调用库函数读取步数和时间
    if (dmp_get_pedometer_step_count(steps) != 0)
    {
        return 1;
    }

    if (dmp_get_pedometer_walk_time(walk_time) != 0)
    {
        return 1;
    }
    return 0;
}

void reset_mpu6050_step_counter(void)
{
    xSemaphoreTake(MutexSemaphore, portMAX_DELAY);  /* 获取互斥信号量 */
    if (dmp_set_pedometer_step_count(0) == 0)
    {
        printf("MPU6050 step counter reset successfully!\r\n");
    }
    else
    {
        printf("Failed to reset MPU6050 step counter!\r\n");
    }
    xSemaphoreGive(MutexSemaphore);                 /* 释放互斥信号量 */
}
