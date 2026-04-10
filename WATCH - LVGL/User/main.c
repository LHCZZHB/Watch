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
#include "./BSP/TOUCH/cst816.h"

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
    // 初始化 SPI 和 LCD，240x280
    spi1_init();
    lcd_init();
    cst816_init();

    /* 开机自检：如果这里都看不到闪灯，说明 PF9/PF10 不是当前板子的 LED 引脚，
       或者硬件连线/极性和这份代码不一致。 */
    for(uint8_t i = 0; i < 3; i++)
    {
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_RESET);
        delay_ms(200);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_SET);
        delay_ms(200);
    }
    // 指定区域填充颜色
    //LCD_Fill(100,100,140,120,LGRAYBLUE);

    // 使用 DMA 指定区域填充颜色
    //uint16_t green_block[1600]; // 40 * 40 = 1600 个像素
    // 初始化颜色数据为绿色 (RGB565: 0x07E0)
    //for(int i = 0; i < 1600; i++) {
    //   green_block[i] = 0x07E0;
    //}
    // 将屏幕 (10, 10) 到 (50, 50) 区域填充
    //LCD_Color_Fill(10, 10, 50, 50, green_block);

    // 指定位置画点
    //LCD_DrawPoint(120, 160, RED);
    
    // 指定位置画线
    //LCD_DrawLine(50, 50, 200, 200, BLUE);

    // 指定位置画矩形
    //LCD_DrawRectangle(60, 60, 180, 180, MAGENTA);

    // 指定位置画圆
    //Draw_Circle(120, 160, 50, CYAN);
    
    // 显示汉字 12x12
    //LCD_ShowChinese12x12(100, 100, (uint8_t *)"你", RED, WHITE, 12, 0);

    // 显示字符
    //LCD_ShowChar(100, 150, 'A', RED, WHITE, 16, 0);

    // 显示字符串    
    //LCD_ShowString(100, 200, (uint8_t *)"Hello, World!", GREEN, BLACK, 16, 0);

    // 显示汉字字符串
    //LCD_ShowChinese(50, 50, (uint8_t *)"你好吗，世界！", YELLOW, BLUE, 16, 0);

    // 显示整数
    //LCD_ShowIntNum(100, 250, 12345, 5, RED, WHITE, 16);
    //LCD_ShowFloatNum1(100, 100, 3.14, 4, RED, WHITE, 16);

    // 显示图片
    //LCD_ShowPicture(0, 0, 240, 280, (const uint8_t *)lcd_image);

    
    uint8_t Key;
    while(1)
    {
        Key = key_scan(0);
        
        if(Key)
        {
            switch(Key)
            {
                case 1:
                    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_RESET);     /* LED0 亮 */
                    break;
                case 2:
                    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET);      /* LED1 亮 */
                    break;
                case 3:
                    HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_10);
                    break;
                case 4:
                    HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_9);
                    break;
                default:
                    break;
            }
        }


        uint8_t finger_num = cst816_get_finger_num();
        if(finger_num != 0x00 && finger_num != 0xFF)
        {
            cst816_get_all_data();
            HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
        }
        

        delay_ms(10);
    }
}

