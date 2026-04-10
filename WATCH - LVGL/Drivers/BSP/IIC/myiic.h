#ifndef __MYIIC_H
#define __MYIIC_H

#include "./SYSTEM/sys/sys.h"

void iic_init(void);
void iic_start(void);
void iic_stop(void);
uint8_t iic_wait_ack(void);
void iic_ack(void);
void iic_nack(void);
void iic_send_byte(uint8_t byte);
uint8_t iic_read_byte(uint8_t ack);
uint8_t iic_device_scan(uint8_t addr_7bit);

uint8_t iic_write_one_byte(uint8_t addr_7bit, uint8_t reg, uint8_t data);
uint8_t iic_read_one_byte(uint8_t addr_7bit, uint8_t reg);
uint8_t iic_write_multi_byte(uint8_t addr_7bit, uint8_t reg, uint8_t length, uint8_t *data);
uint8_t iic_read_multi_byte(uint8_t addr_7bit, uint8_t reg, uint8_t length, uint8_t *data);
#endif
