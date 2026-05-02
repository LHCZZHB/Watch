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
#include "./BSP/WWDG/wwdg.h"
#include "lvgl_demo.h"
#include "freertos_demo.h"




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
    wwdg_init(0x7F, 0x50, WWDG_PRESCALER_8); /* 初始化窗口看门狗 */
    freertos_demo();
    

    while(1)
    {

    }
}

