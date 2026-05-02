#include "./BSP/WWDG/wwdg.h"
#include "./BSP/LED/led.h"

WWDG_HandleTypeDef wwdg_handler;        /* 窗口看门狗句柄 */

/**
 * @brief       初始化窗口看门狗
 * @param       tr: T[6:0],计数器值
 * @param       tw: W[6:0],窗口上限值值
 * @note        fprer:分频系数（WDGTB）,范围:WWDG_PRESCALER_1~WWDG_PRESCALER_8,表示2^WDGTB分频
 *              Fwwdg=PCLK1/(4096*2^fprer). 一般PCLK1=42Mhz
 * @retval      无
 */
void wwdg_init(uint8_t tr, uint8_t wr, uint32_t fprer)
{
    wwdg_handler.Instance = WWDG;
    wwdg_handler.Init.Prescaler = fprer;         /* 设置分频系数 */
    wwdg_handler.Init.Window = wr;               /* 设置窗口值 */
    wwdg_handler.Init.Counter = tr;              /* 设置计数器值 */
    wwdg_handler.Init.EWIMode = WWDG_EWI_ENABLE; /* 使能窗口看门狗提前唤醒中断 */
    HAL_WWDG_Init(&wwdg_handler);                /* 初始化WWDG */
}

/**
 * @brief       WWDG底层驱动，时钟配置，中断配置
                此函数会被HAL_WWDG_Init()调用
 * @param       hwwdg:窗口看门狗句柄
 * @retval      无
 */
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
    __HAL_RCC_WWDG_CLK_ENABLE();            /* 使能窗口看门狗时钟 */

    HAL_NVIC_SetPriority(WWDG_IRQn, 2, 0);  /* 抢占优先级2，子优先级为0 */
    HAL_NVIC_EnableIRQ(WWDG_IRQn);          /* 使能窗口看门狗中断 */
}

/**
 * @brief       窗口看门狗中断服务程序
 * @param       无
 * @retval      无
 */
void WWDG_IRQHandler(void)
{
    HAL_WWDG_IRQHandler(&wwdg_handler);     /* 调用WWDG共用中断处理函数 */
}

/**
 * @brief       中断服务函数处理过程
                此函数会被HAL_WWDG_IRQHandler()调用
 * @param       无
 * @retval      无
 */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    HAL_WWDG_Refresh(&wwdg_handler);        /* 更新窗口看门狗值 */
}

