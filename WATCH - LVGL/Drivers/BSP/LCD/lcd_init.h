#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "./SYSTEM/sys/sys.h"

#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 280

#else
#define LCD_W 280
#define LCD_H 240
#endif


//-----------------LCD端口定义---------------- 
#define SCLK_PORT			GPIOB
#define SCLK_PIN			GPIO_PIN_3

#define SDA_PORT			GPIOB
#define SDA_PIN				GPIO_PIN_5

#define RES_PORT			GPIOB
#define RES_PIN				GPIO_PIN_1

#define DC_PORT				GPIOB
#define DC_PIN				GPIO_PIN_2

#define CS_PORT				GPIOB
#define CS_PIN				GPIO_PIN_6

//#define BLK_PORT			GPIOB
//#define BLK_PIN				GPIO_PIN_0

#define LCD_SCLK_Clr() HAL_GPIO_WritePin(SCLK_PORT,SCLK_PIN,GPIO_PIN_RESET)//SCL=SCLK
#define LCD_SCLK_Set() HAL_GPIO_WritePin(SCLK_PORT,SCLK_PIN,GPIO_PIN_SET)

#define LCD_MOSI_Clr() HAL_GPIO_WritePin(SDA_PORT,SDA_PIN,GPIO_PIN_RESET)//SDA=MOSI
#define LCD_MOSI_Set() HAL_GPIO_WritePin(SDA_PORT,SDA_PIN,GPIO_PIN_SET)

#define LCD_RES_Clr()  HAL_GPIO_WritePin(RES_PORT,RES_PIN,GPIO_PIN_RESET)//RES
#define LCD_RES_Set()  HAL_GPIO_WritePin(RES_PORT,RES_PIN,GPIO_PIN_SET)

#define LCD_DC_Clr()   HAL_GPIO_WritePin(DC_PORT,DC_PIN,GPIO_PIN_RESET)//DC
#define LCD_DC_Set()   HAL_GPIO_WritePin(DC_PORT,DC_PIN,GPIO_PIN_SET)
 		     
#define LCD_CS_Clr()   HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_RESET)//CS
#define LCD_CS_Set()   HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_SET)

//#define LCD_BLK_Clr()  HAL_GPIO_WritePin(BLK_PORT,BLK_PIN,GPIO_PIN_RESET)//BLK
//#define LCD_BLK_Set()  HAL_GPIO_WritePin(BLK_PORT,BLK_PIN,GPIO_PIN_SET)

void lcd_gpio_init(void);
//LCD串行数据写入函数(software SPI)
void lcd_writ_bus(uint8_t data);

//LCD写入数据
void lcd_wr_data8(uint8_t data);

//LCD写入数据
void lcd_wr_data(uint16_t data);

//LCD写入命令
void lcd_wr_rcom(uint8_t data);
//设置起始和结束地址
void lcd_address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);

//ST7789 SLEEP IN
void lcd_st7789_sleepin(void);

//ST7789 SLEEP OUT
void lcd_st7789_sleepout(void);

//LCD初始化
void lcd_init(void);
#endif
