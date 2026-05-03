#ifndef __MPU6050_H
#define __MPU6050_H

#include "./SYSTEM/sys/sys.h"



//#define MPU_XDA_PORT			GPIO
//#define MPU_XDA_PIN		        GPIO_PIN_14

//#define MPU_XCL_PORT			GPIOB
//#define MPU_XCL_PIN		        GPIO_PIN_13


//设备地址
#define Device_Addr_mpu 	0x68
//设备写地址
#define Device_Write_Addr_mpu 	0xD0
//设备读地址
#define Device_Read_Addr_mpu 	0xD1

#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C
#define INT_PIN_CFG          0x37       // 中断引脚配置[cite: 1]
#define INT_ENABLE           0x38       // 中断使能[cite: 1]
#define INT_STATUS           0x3A       // 中断状态[cite: 1]

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75

void mpu6050_init(void); /* MPU6050初始化 */
void mpu6050_getdata(int16_t *accel_data, int16_t *gyro_data); /* 获取加速度和陀螺仪数据 */


#endif
