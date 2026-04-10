#include "./BSP/IIC/myiic.h"
#include "./BSP/TOUCH/cst816.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"

CST816_Info CST816_Instance;
// CST816 GPIO 初始化
void cst816_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 初始化复位引脚和中断引脚
    GPIO_InitStruct.Pin = TOUCH_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(TOUCH_RST_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_SET); // 复位引脚默认高电平

    GPIO_InitStruct.Pin = TOUCH_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TOUCH_INT_PORT, &GPIO_InitStruct);
}

// CST816 初始化
uint8_t cst816_init(void)
{
    if(iic_device_scan(0x15) != 0)
    {
        // 设备未找到
        return 1;
    }

    printf("CST816 device found!\r\n");

    cst816_gpio_init();
    // 读取芯片 ID，验证通信是否正常
    uint8_t id = cst816_get_id();
    if(id != ChipID)
    {
        printf("CST816 id error\r\n");
        // 芯片 ID 不正确
        return 1;
    }
    printf("CST816 id success\r\n");
    return 0; // 初始化成功
}


// 读取触摸屏单个寄存器的数据
uint8_t cst816_iic_read_reg(uint8_t reg)
{
    return iic_read_one_byte(Device_Addr_cst, reg);
}

// 写入触摸屏单个寄存器的数据
uint8_t cst816_iic_write_reg(uint8_t reg, uint8_t data)
{
    return iic_write_one_byte(Device_Addr_cst, reg, data);
}

// 触摸屏复位
void cst816_reset(void)
{
    TOUCH_RST_RES(); // 拉低复位引脚
    delay_ms(10); // 延时 10 毫秒
    TOUCH_RST_SET(); // 拉高复位引脚
    delay_ms(100); // 延时 100 毫秒，等待芯片复位完成
}


// 读取触摸屏触摸时的坐标值
void cst816_get_all_data(void)
{
    uint8_t data[6];
    // 从 GestureID(0x01) 开始连续读取 6 个字节
    if(iic_read_multi_byte(Device_Addr_cst, 0x01, 6, data) == 0)
    {
//        CST816_Instance.Gesture = data[0];      // 手势 ID
//        CST816_Instance.FingerNum = data[1];    // 手指个数
        // 坐标计算逻辑不变
        CST816_Instance.X_Pos = ((data[2] & 0x0F) << 8) | data[3];
        CST816_Instance.Y_Pos = ((data[4] & 0x0F) << 8) | data[5];
    }
}
// 读取触摸屏的手指触摸个数，0xFF 表示睡眠
uint8_t cst816_get_finger_num(void)
{
    return cst816_iic_read_reg(FingerNum);
}

// 读取触摸屏芯片 ID
uint8_t cst816_get_id(void)
{
    return cst816_iic_read_reg(ChipID);
}

// 使能连续动作
void cst816_enable_continuous_mode(uint8_t mode)
{
    cst816_iic_write_reg(MotionMask, mode); // 设置 MotionMask 寄存器
}


// 在指定时间内无触摸后自动进入低功耗模式
void cst816_config_auto_sleep_time(uint8_t time)
{
    cst816_iic_write_reg(AutoSleepTime, time); // 设置 AutoSleepTime
}

// 进入睡眠，无触摸唤醒功能
void cst816_sleep(void)
{
    cst816_iic_write_reg(SleepMode, 0x03); // 设置 SleepMode
}

// 唤醒
void cst816_wakeup(void)
{
    cst816_reset(); // 通过复位唤醒触摸屏
}   

// 手势检测滑动分区角度控制。Angle = tan(c) * 10，c 为相对 x 轴正方向的角度。
void cst816_config_motion_sl_angle(uint8_t x_right_y_up_angle)
{
    cst816_iic_write_reg(MotionSlAngle, x_right_y_up_angle); // 设置 MotionSlAngle
}

// 正常快速检测周期配置函数。单位 10ms，可选值 1～30，默认值为 1。
void cst816_config_nor_scan_per(uint8_t period)
{
    if(period >= 30)
        period = 30; // 限制周期值范围
    cst816_iic_write_reg(NorScanPer, period); // 设置 NorScanPer
}


// 中断低脉冲宽度配置
void cst816_config_irq_pluse_width(uint8_t Width)
{
	if(Width >= 200)
		Width = 200;
	cst816_iic_write_reg(IrqPluseWidth, Width);
}

// 低功耗扫描唤醒阈值配置
void cst816_config_lp_scan_th(uint8_t TH)
{
	cst816_iic_write_reg(LpScanTH, TH);
}
