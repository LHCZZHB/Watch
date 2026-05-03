#ifndef __EXTI_H
#define __EXTI_H

#include "./SYSTEM/sys/sys.h"


#define TOUCH_INT_PORT			GPIOB
#define TOUCH_INT_PIN		    GPIO_PIN_15

#define TOUCH_INT_RES()   HAL_GPIO_WritePin(TOUCH_INT_PORT,TOUCH_INT_PIN,GPIO_PIN_RESET)//INT
#define TOUCH_INT_SET()   HAL_GPIO_WritePin(TOUCH_INT_PORT,TOUCH_INT_PIN,GPIO_PIN_SET)
 		    
#define MPU_INT_PORT			GPIOB
#define MPU_INT_PIN		        GPIO_PIN_12


#define MPU_INT_RES()   HAL_GPIO_WritePin(MPU_INT_PORT,MPU_INT_PIN,GPIO_PIN_RESET)//INT
#define MPU_INT_SET()   HAL_GPIO_WritePin(MPU_INT_PORT,MPU_INT_PIN,GPIO_PIN_SET)


void extix_init(void);  /* 外部中断初始化 */

#endif

























