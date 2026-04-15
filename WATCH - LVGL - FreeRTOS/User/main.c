#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/IIC/myiic.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/LCD/lcd_init.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/DMA/dma.h"
#include "lvgl_demo.h"

//#include "lv_demo_stress.h"
//#include "lv_demo_music.h"

volatile uint8_t cst_itr = 0; // 触摸中断标志位

int main(void)
{

    HAL_Init();                         /* 初始化 HAL */
    sys_stm32_clock_init(336, 8, 2, 7); /* 配置系统时钟，168 MHz */
    delay_init(168);                    /* 延时初始化 */
    usart_init(115200);
    led_init();                         /* 初始化 LED */
    key_init();
    iic_init();                         /* 初始化 IIC，用于触摸屏 */
    dma2_init();                        /* 初始化 DMA2 */
    spi1_init();

//    lv_obj_t* switch_obj = lv_switch_create(lv_scr_act());
//    lv_obj_set_size(switch_obj, 120, 60);
//    lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);

//    lv_demo_stress();
//    lv_demo_music();

    lvgl_demo();
    
    uint16_t x, y;
    uint8_t gesture;
    uint8_t finger_num;
    while(1)
    {

        
        if(cst_itr == 1) // 如果触摸中断标志位被设置
        {
            cst_itr = 0; // 清除触摸中断标志位
            LED1_TOGGLE(); // 触摸事件发生时切换 LED 状态
            cst816t_getaction(&x, &y, &gesture, &finger_num); // 获取触摸状态
        }                    /* LVGL 任务处理函数 */
    }
}

