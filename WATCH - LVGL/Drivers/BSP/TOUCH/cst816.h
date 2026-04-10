#ifndef __CST816_H
#define __CST816_H

#include "./SYSTEM/sys/sys.h"


#define TOUCH_RST_PORT			GPIOB
#define TOUCH_RST_PIN		    GPIO_PIN_14

#define TOUCH_INT_PORT			GPIOB
#define TOUCH_INT_PIN		    GPIO_PIN_15

#define TOUCH_RST_RES()  HAL_GPIO_WritePin(TOUCH_RST_PORT,TOUCH_RST_PIN,GPIO_PIN_RESET)//RES
#define TOUCH_RST_SET()  HAL_GPIO_WritePin(TOUCH_RST_PORT,TOUCH_RST_PIN,GPIO_PIN_SET)

#define TOUCH_INT_RES()   HAL_GPIO_WritePin(TOUCH_INT_PORT,TOUCH_INT_PIN,GPIO_PIN_RESET)//INT
#define TOUCH_INT_SET()   HAL_GPIO_WritePin(TOUCH_INT_PORT,TOUCH_INT_PIN,GPIO_PIN_SET)
 		    
/* 设备地址 */
#define Device_Addr_cst 	0x15


/* 触摸屏寄存器地址 */
#define GestureID 			0x01
#define FingerNum 			0x02
#define XposH 					0x03
#define XposL 					0x04
#define YposH 					0x05
#define YposL 					0x06
#define ChipID 					0xFF
#define SleepMode				0xE5
#define MotionMask 			0xEC
#define IrqPluseWidth 	0xED
#define NorScanPer 			0xEE
#define MotionSlAngle 	0xEF
#define LpAutoWakeTime 	0xF4
#define LpScanTH 				0xF5
#define LpScanWin 			0xF6
#define LpScanFreq 			0xF7
#define LpScanIdac 			0xF8
#define AutoSleepTime 	0xF9
#define IrqCtl 					0xFA
#define AutoReset 			0xFB
#define LongPressTime 	0xFC
#define IOCtl 					0xFD
#define DisAutoSleep 		0xFE

/* 触摸信息结构体 */
typedef struct
{
	unsigned int X_Pos;
	unsigned int Y_Pos;
//	uint8_t FingerNum;
//	uint8_t Gesture;
} CST816_Info;

extern CST816_Info CST816_Instance;

/* 手势 ID 识别选项 */
typedef enum
{
	NOGESTURE = 	0x00,
	DOWNGLIDE = 	0x01,
	UPGLIDE = 		0x02,
	LEFTGLIDE = 	0x03,
	RIGHTGLIDE = 	0x04,
	CLICK = 			0x05,
	DOUBLECLICK = 0x0B,
	LONGPRESS = 	0x0C,
} GestureID_TypeDef;

/* 运动使能选项 */
typedef enum
{
	M_DISABLE = 	0x00,
	EnConLR = 		0x01,
	EnConUD = 		0x02,
	EnDClick = 		0x03,
	M_ALLENABLE = 0x07,
} MotionMask_TypeDef;

/* 中断触发方式选项 */
typedef enum
{
	OnceWLP = 		0x00,
	EnMotion = 		0x10,
	EnChange = 		0x20,
	EnTouch = 		0x40,
	EnTest = 			0x80,
} IrqCtl_TypeDef;

uint8_t cst816_init(void);
void cst816_reset(void);
void cst816_get_all_data(void);
uint8_t cst816_get_finger_num(void);
uint8_t cst816_get_id(void);
void cst816_enable_continuous_mode(uint8_t mode);
void cst816_config_auto_sleep_time(uint8_t time);
void cst816_sleep(void);
void cst816_wakeup(void);
void cst816_config_motion_sl_angle(uint8_t x_right_y_up_angle);
void cst816_config_nor_scan_per(uint8_t period);
void cst816_config_irq_pluse_width(uint8_t Width);
void cst816_config_lp_scan_th(uint8_t TH);

#endif 
