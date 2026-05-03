#include "./BSP/IIC/myiic.h"
#include "./BSP/TOUCH/cst816.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/MPU6050/mpu6050.h"

CST816_Info CST816_Instance;
// CST816 GPIO 初�?�化
void cst816t_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 初�?�化复位引脚和中�?引脚
    GPIO_InitStruct.Pin = TOUCH_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(TOUCH_RST_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_SET); // 复位引脚默�?�高电平

}


//CST816读取一�?字节
uint8_t cst816t_receivebyte(uint8_t regaddress)
{
    uint8_t data;

    iic_start();
    iic_send_byte(Device_Write_Addr_cst); // 发送�?��?�写地址
    iic_wait_ack();
    iic_send_byte(regaddress); // 发送寄存器地址
    iic_wait_ack();

    iic_start(); // 重新发送起始信�?
    iic_send_byte(Device_Read_Addr_cst); // 发送�?��?��?�地址
    iic_wait_ack();
    data = iic_read_byte(0); // 读取单字节后发�? NACK
    iic_stop(); // 发送停�?信号

    return data;
}

//CST816连续读取多个字节
uint8_t cst816t_receivemultibytes(uint8_t regaddress, uint8_t *data, uint8_t length)
{
    iic_start();
    iic_send_byte(Device_Write_Addr_cst); // 发送�?��?�写地址
    iic_wait_ack();
    iic_send_byte(regaddress); // 发送寄存器地址
    iic_wait_ack();

    iic_start(); // 重新发送起始信�?
    iic_send_byte(Device_Read_Addr_cst); // 发送�?��?��?�地址
    iic_wait_ack();

    for(uint8_t i = 0; i < length; i++)
    {
        data[i] = iic_read_byte(i < length - 1); // 接收数据，最后一�?字节发�? NACK
    }
    
    iic_stop(); // 发送停�?信号
    return length; // 成功读取的字节数
}

//CST816写入单个字节
void cst816t_sendbyte(uint8_t regaddress, uint8_t data)
{
    iic_start();
    iic_send_byte(Device_Write_Addr_cst); // 发送�?��?�写地址
    iic_wait_ack();
    iic_send_byte(regaddress); // 发送寄存器地址
    iic_wait_ack();
    iic_send_byte(data); // 发送数�?
    iic_wait_ack();
    iic_stop(); // 发送停�?信号
}

// CST816 初�?�化
void cst816t_init(void)
{
    cst816t_gpio_init();
    TOUCH_RST_RES(); // 拉低复位引脚
    delay_ms(10); // 延时 10 �?�?
    TOUCH_RST_SET(); // 拉高复位引脚
    delay_ms(100); // 延时 100 �?秒，等待�?片�?�位完成

    uint8_t chip_id = cst816t_receivebyte(ChipID);
    if(chip_id == 0xB5)
    {
        printf("CST816 initialization successful!\r\n");
    }
    else
    {
        printf("CST816 initialization failed! Read Chip ID: 0x%02X\r\n", chip_id);
    }
    cst816t_sendbyte(IrqCtl, 0x70); // 默�?�配�?：使能所有触摸事件中�?，低脉冲发射方式
}

//状态�?�取
void cst816t_getaction(uint16_t *x, uint16_t *y, uint8_t *gesture, uint8_t *finger_num)
{
    uint8_t data[6];
    cst816t_receivemultibytes(GestureID, data, 6);

    *gesture = data[0];
    *finger_num = data[1];

    if(data[1] == 0x00 || data[1] == 0xFF)
    {
         *x = 0;
         *y = 0;
         *gesture = 0;
        *finger_num = 0;
         //printf("gesture=0, X=0, Y=0\r\n");
         return;
    }

    *x = (uint16_t)((data[2] & 0x0F) << 8) | data[3];
    *y = (uint16_t)((data[4] & 0x0F) << 8) | data[5];
    //printf("gesture=%u, X=%u, Y=%u\r\n", *gesture, *x, *y);
}




//// 触摸屏�?�位
//void cst816_reset(void)
//{
//    TOUCH_RST_RES(); // 拉低复位引脚
//    delay_ms(10); // 延时 10 �?�?
//    TOUCH_RST_SET(); // 拉高复位引脚
//    delay_ms(100); // 延时 100 �?秒，等待�?片�?�位完成
//}

//// 使能连续动作
//void cst816_enable_continuous_mode(uint8_t mode)
//{
//    cst816_iic_write_reg(MotionMask, mode); // 设置 MotionMask 寄存�?
//}


//// 在指定时间内无触摸后�?动进入低功耗模�?
//void cst816_config_auto_sleep_time(uint8_t time)
//{
//    cst816_iic_write_reg(AutoSleepTime, time); // 设置 AutoSleepTime
//}

//// 进入睡眠，无触摸唤醒功能
//void cst816_sleep(void)
//{
//    cst816_iic_write_reg(SleepMode, 0x03); // 设置 SleepMode
//}

//// 唤醒
//void cst816_wakeup(void)
//{
//    cst816_reset(); // 通过复位唤醒触摸�?
//}   

//// 手势检测滑动分区�?�度控制。Angle = tan(c) * 10，c 为相�? x 轴�?�方向的角度�?
//void cst816_config_motion_sl_angle(uint8_t x_right_y_up_angle)
//{
//    cst816_iic_write_reg(MotionSlAngle, x_right_y_up_angle); // 设置 MotionSlAngle
//}

//// 正常�?速�?�测周期配�?函数。单�? 10ms，可选�? 1�?30，默认值为 1�?
//void cst816_config_nor_scan_per(uint8_t period)
//{
//    if(period >= 30)
//        period = 30; // 限制周期值范�?
//    cst816_iic_write_reg(NorScanPer, period); // 设置 NorScanPer
//}


//// �?�?低脉冲�?�度配置
//void cst816_config_irq_pluse_width(uint8_t Width)
//{
//	if(Width >= 200)
//		Width = 200;
//	cst816_iic_write_reg(IrqPluseWidth, Width);
//}

//// 低功耗扫描唤醒阈值配�?
//void cst816_config_lp_scan_th(uint8_t TH)
//{
//	cst816_iic_write_reg(LpScanTH, TH);
//}
