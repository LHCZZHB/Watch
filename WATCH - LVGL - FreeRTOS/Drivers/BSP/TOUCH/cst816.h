#ifndef __CST816_H
#define __CST816_H

#include "./SYSTEM/sys/sys.h"

extern volatile uint8_t cst_itr;


#define TOUCH_RST_PORT			GPIOB
#define TOUCH_RST_PIN		    GPIO_PIN_14

#define TOUCH_INT_PORT			GPIOB
#define TOUCH_INT_PIN		    GPIO_PIN_15

#define TOUCH_RST_RES()  HAL_GPIO_WritePin(TOUCH_RST_PORT,TOUCH_RST_PIN,GPIO_PIN_RESET)//RES
#define TOUCH_RST_SET()  HAL_GPIO_WritePin(TOUCH_RST_PORT,TOUCH_RST_PIN,GPIO_PIN_SET)

#define TOUCH_INT_RES()   HAL_GPIO_WritePin(TOUCH_INT_PORT,TOUCH_INT_PIN,GPIO_PIN_RESET)//INT
#define TOUCH_INT_SET()   HAL_GPIO_WritePin(TOUCH_INT_PORT,TOUCH_INT_PIN,GPIO_PIN_SET)
 		    
//设备地址
#define Device_Addr_cst 	0x15
//设备写地址
#define Device_Write_Addr_cst 	0x2A
//设备读地址
#define Device_Read_Addr_cst 	0x2B


/* 触摸屏寄存器 */
#define GestureID 			0x01
#define FingerNum 			0x02
#define XposH 					0x03
#define XposL 					0x04
#define YposH 					0x05
#define YposL 					0x06
#define ChipID 					0xA7
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

/* 触摸屏坐标结构体 */
typedef struct
{
	unsigned int X_Pos;
	unsigned int Y_Pos;
} CST816_Info;

extern CST816_Info CST816_Instance;

//typedef struct {
//    uint32_t first_click_time;
//    uint16_t first_x;
//    uint16_t first_y;
//    uint8_t click_stage;
//    uint8_t keep_dblclick;       // 新增双击保持标志
//    uint8_t last_valid_gesture;  // 记录有效手势
//} DoubleClickState;
//static DoubleClickState dc_state = {0};

/* 手势ID识别选项 */
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

/* 连续动作配置选项 */
typedef enum
{
	M_DISABLE = 	0x00,
	EnConLR = 		0x01,
	EnConUD = 		0x02,
	EnDClick = 		0x03,
	M_ALLENABLE = 0x07,
} MotionMask_TypeDef;

/* 中断低脉冲发射方式选项 */
typedef enum
{
	OnceWLP = 		0x00,
	EnMotion = 		0x10,
	EnChange = 		0x20,
	EnTouch = 		0x40,
	EnTest = 			0x80,
} IrqCtl_TypeDef;

void cst816t_init(void);
uint8_t cst816t_receivebyte(uint8_t regaddress);
uint8_t cst816t_receivemultibytes(uint8_t regaddress, uint8_t *data, uint8_t length);
void cst816t_sendbyte(uint8_t regaddress, uint8_t data);
void cst816t_getaction(uint16_t *x, uint16_t *y, uint8_t *gesture, uint8_t *finger_num);
#endif 
