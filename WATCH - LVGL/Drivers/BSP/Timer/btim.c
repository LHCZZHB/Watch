#include "./BSP/Timer/btim.h"
#include "./SYSTEM/USART/usart.h"
#include "lvgl.h"

TIM_HandleTypeDef g_tim6_handle;        /* 定时器6句柄 */
TIM_HandleTypeDef g_tim7_handle;        /* 定时器7句柄 */

/**
 * @brief       基本定时器TIM6中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIM6_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim6_handle);  /*定时器回调函数*/
}

/**
 * @brief       基本定时器TIM7中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIM7_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim7_handle);  /*定时器回调函数*/
}

/**
 * @brief       回调函数，定时器中断服务函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_tim6_handle))
    {
        lv_tick_inc(1);
    }
    
}

//定时器6中断初始化函数
void btim_tim6_int_init(uint16_t arr,uint16_t psc)
{
    
    g_tim6_handle.Instance = BTIM_TIM6_INT;
    g_tim6_handle.Init.Prescaler = psc;//预分频系数
    g_tim6_handle.Init.Period = arr;//自动重载值ARR
    g_tim6_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;//自动重载预装载使能
    HAL_TIM_Base_Init(&g_tim6_handle);
    
    HAL_TIM_Base_Start_IT(&g_tim6_handle);
}

//定时器7中断初始化函数
void btim_tim7_int_init(uint16_t arr,uint16_t psc)
{
    
    g_tim7_handle.Instance = BTIM_TIM7_INT;
    g_tim7_handle.Init.Prescaler = psc;//预分频系数
    g_tim7_handle.Init.Period = arr;//自动重载值ARR
    g_tim7_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;//自动重载预装载使能
    HAL_TIM_Base_Init(&g_tim7_handle);
    
    HAL_TIM_Base_Start_IT(&g_tim7_handle);
}
//定时器基础MSP初始化函数
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIM6_INT)
    {
        BTIM_TIM6_INT_CLK_ENABLE();                     /*使能TIM时钟*/
        HAL_NVIC_SetPriority(BTIM_TIM6_INT_IRQn, 4, 0); /* 抢占4，子优先级0，组4 */
        HAL_NVIC_EnableIRQ(BTIM_TIM6_INT_IRQn);         /*开启ITM3中断*/
    }
    else if (htim->Instance == BTIM_TIM7_INT)
    {
        BTIM_TIM7_INT_CLK_ENABLE();                     /*使能TIM时钟*/
        HAL_NVIC_SetPriority(BTIM_TIM7_INT_IRQn, 6, 0); /* 抢占6，子优先级0，组4 */
        HAL_NVIC_EnableIRQ(BTIM_TIM7_INT_IRQn);         /*开启ITM5中断*/
    }
}


