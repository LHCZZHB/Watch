#include "./BSP/SPI/spi.h"
#include "./BSP/LCD/lcd_init.h"
#include "./SYSTEM/delay/delay.h"

extern SPI_HandleTypeDef g_spi1_handle;//SPI句柄

//LCD端口初始化
void lcd_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    
 	__HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitStructure.Pin = RES_PIN|CS_PIN|DC_PIN;	 
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//速度50MHz
 	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOB
 	HAL_GPIO_WritePin(GPIOB, RES_PIN|CS_PIN|DC_PIN, GPIO_PIN_SET);
}

//LCD串行数据写入函数(software SPI)
void lcd_writ_bus(uint8_t data) 
{	
    HAL_SPI_Transmit(&g_spi1_handle,&data,1,1000);
}

//LCD写入数据
void lcd_wr_data8(uint8_t data)
{
    lcd_writ_bus(data);
}

//LCD写入颜色数据
void lcd_wr_data(uint16_t data)
{
    uint8_t temp[2];
    temp[0]=(data>>8)&0xff;
    temp[1]=data&0xff;
    HAL_SPI_Transmit(&g_spi1_handle,temp,2,1000);
}

//LCD写入命令
void lcd_wr_rcom(uint8_t data)
{
    LCD_DC_Clr();//写命令
    lcd_writ_bus(data);
    LCD_DC_Set();//写数据
}

//设置起始和结束地址
void lcd_address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
    lcd_wr_rcom(0x2a);//列地址设置
    lcd_wr_data(x1);
    lcd_wr_data(x2);
    lcd_wr_rcom(0x2b);//行地址设置
    lcd_wr_data(y1);
    lcd_wr_data(y2);
    lcd_wr_rcom(0x2c);//储存器写
}

//ST7789 SLEEP IN
void lcd_st7789_sleepin(void)
{
    lcd_wr_rcom(0x28);
    delay_ms(20);
    lcd_wr_rcom(0x10);
    delay_ms(120);
}   

//ST7789 SLEEP OUT
void lcd_st7789_sleepout(void)
{
    lcd_wr_rcom(0x11);
    delay_ms(120);
    lcd_wr_rcom(0x29);
}

//LCD初始化
void lcd_init(void)
{
    lcd_gpio_init(); //初始化GPIO
    LCD_CS_Clr();		//低电平有效

    LCD_RES_Clr(); 
    delay_ms(100);
    LCD_RES_Set();
    delay_ms(100);
    
    lcd_wr_rcom(0x11); 
    delay_ms(120); //Delay 120ms

    lcd_wr_rcom(0x36); 
    if(USE_HORIZONTAL==0)lcd_wr_data8(0x00);
    else if(USE_HORIZONTAL==1)lcd_wr_data8(0xC0);
    else if(USE_HORIZONTAL==2)lcd_wr_data8(0x70);
    else lcd_wr_data8(0xA0);

    lcd_wr_rcom(0x3A);     
    lcd_wr_data8(0x55);   

    lcd_wr_rcom(0xB2);     
    lcd_wr_data8(0x0B);   
    lcd_wr_data8(0x0B);   
    lcd_wr_data8(0x00);   
    lcd_wr_data8(0x33);   
    lcd_wr_data8(0x33);   

    lcd_wr_rcom(0xB7);     
    lcd_wr_data8(0x75);   //11

    lcd_wr_rcom(0xBB);     
    lcd_wr_data8(0x2b);   //2f

    lcd_wr_rcom(0xC0);     
    lcd_wr_data8(0x2C);   

    lcd_wr_rcom(0xC2);     
    lcd_wr_data8(0x01);   

    lcd_wr_rcom(0xC3);     
    lcd_wr_data8(0x16);  //0d 

    lcd_wr_rcom(0xC4);     
    lcd_wr_data8(0x20);   //VDV, 0x20:0v

    lcd_wr_rcom(0xC6);     
    lcd_wr_data8(0x13);   //0x13:60Hz   

    lcd_wr_rcom(0xD0);     
    lcd_wr_data8(0xA4);   
    lcd_wr_data8(0xA1);   

    lcd_wr_rcom(0xD6);     
    lcd_wr_data8(0xA1);   

    lcd_wr_rcom(0xE0);     
    lcd_wr_data8(0xF0);   
    lcd_wr_data8(0x04);   
    lcd_wr_data8(0x07);   
    lcd_wr_data8(0x09);   
    lcd_wr_data8(0x07);   
    lcd_wr_data8(0x13);   
    lcd_wr_data8(0x25);   
    lcd_wr_data8(0x33);   
    lcd_wr_data8(0x3C);   
    lcd_wr_data8(0x34);   
    lcd_wr_data8(0x10);   
    lcd_wr_data8(0x10);   
    lcd_wr_data8(0x29);   
    lcd_wr_data8(0x32);   

    lcd_wr_rcom(0xE1);     
    lcd_wr_data8(0xF0);   
    lcd_wr_data8(0x05);   
    lcd_wr_data8(0x08);   
    lcd_wr_data8(0x0A);   
    lcd_wr_data8(0x09);   
    lcd_wr_data8(0x05);   
    lcd_wr_data8(0x25);   
    lcd_wr_data8(0x32);   
    lcd_wr_data8(0x3B);   
    lcd_wr_data8(0x3B);   
    lcd_wr_data8(0x17);   
    lcd_wr_data8(0x18);   
    lcd_wr_data8(0x2E);   
    lcd_wr_data8(0x37);   

    lcd_wr_rcom(0xE4);     
    lcd_wr_data8(0x25);   
    lcd_wr_data8(0x00);   
    lcd_wr_data8(0x00);   

    lcd_wr_rcom(0x21);   

    lcd_wr_rcom(0x29);
    delay_ms(50);     

    lcd_wr_rcom(0x2c); 
}
