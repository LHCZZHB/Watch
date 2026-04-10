#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"

#define KEY_UP      HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
#define KEY0        HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4)
#define KEY1        HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3)
#define KEY2        HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2)

void key_init(void);
uint8_t key_scan(uint8_t mode);

#endif
