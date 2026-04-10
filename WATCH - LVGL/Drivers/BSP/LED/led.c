#include "./BSP/LED/led.h"

void led_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    __HAL_RCC_GPIOF_CLK_ENABLE();
    
    gpio_init_struct.Pin = GPIO_PIN_9;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOF, &gpio_init_struct);
    
    gpio_init_struct.Pin = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOF, &gpio_init_struct);
    
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_SET);
}
