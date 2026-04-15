#ifndef __BTIM_H
#define __BTIM_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 基本定时器 头文件 */

/* TIMX 中断定义
 * 默认使用 TIM6/TIM7
 * 注: 通过修改下面4个宏定义, 可以支持 TIM1~TIM8 之间任一基本定时器.
 */
 
#define BTIM_TIM6_INT                       TIM6
#define BTIM_TIM6_INT_IRQn                  TIM6_DAC_IRQn
#define BTIM_TIM6_INT_IRQHandler            TIM6_DAC_IRQHandler
#define BTIM_TIM6_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM6_CLK_ENABLE(); }while(0)  /* TIM6 时钟使能 */

#define BTIM_TIM7_INT                       TIM7
#define BTIM_TIM7_INT_IRQn                  TIM7_IRQn
#define BTIM_TIM7_INT_IRQHandler            TIM7_IRQHandler
#define BTIM_TIM7_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM7_CLK_ENABLE(); }while(0)  /* TIM7 时钟使能 */

/******************************************************************************************/

void btim_tim6_int_init(uint16_t arr, uint16_t psc);
void btim_tim7_int_init(uint16_t arr, uint16_t psc);

#endif
