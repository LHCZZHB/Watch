#include "./BSP/KEY/key.h"
#include "./SYSTEM/delay/delay.h"

void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    
    gpio_init_struct.Pin = GPIO_PIN_0;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_PULLDOWN;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);
    
    gpio_init_struct.Pin = GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2;
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE, &gpio_init_struct);
    
}

//mode=1,????????????
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1;
    uint8_t key_state = 0;
    
    if(mode) key_up = 1;
    
    if(key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 ==0 || KEY_UP == 1))
    {
        delay_ms(10);
        key_up = 0;
        
        if(KEY_UP == 1) key_state = 1;
        if(KEY0 == 0) key_state = 2;
        if(KEY1 == 0) key_state = 3;
        if(KEY2 == 0) key_state = 4;
    }
    else if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && KEY_UP == 0)
    {
        key_up = 1;
    }
    return key_state;
}
