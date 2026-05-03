#ifndef __MYIIC_H
#define __MYIIC_H

#include "./SYSTEM/delay/delay.h"

#include "./SYSTEM/sys/sys.h"

#define SCL_H() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET)   // SCL 拉高
#define SCL_L() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET) // SCL 拉低

#define SDA_H() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)   // SDA 拉高
#define SDA_L() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET) // SDA 拉低

#define SDA_READ() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) // 读取 SDA 状态

void iic_init(void);
void iic_start(void);
void iic_stop(void);
uint8_t iic_wait_ack(void);
void iic_ack(void);
void iic_nack(void);
void iic_send_byte(uint8_t byte);
uint8_t iic_read_byte(uint8_t ack);

#endif
