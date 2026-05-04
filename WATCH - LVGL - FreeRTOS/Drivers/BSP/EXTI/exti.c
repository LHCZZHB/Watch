#include "./SYSTEM/sys/sys.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "./SYSTEM/delay/delay.h"
#include "./BSP/EXTI/exti.h"
#include "./BSP/LED/led.h"

extern SemaphoreHandle_t MpuSemaphore;
extern SemaphoreHandle_t TouchSemaphore;

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(TOUCH_INT_PIN); // 处理触摸????
    __HAL_GPIO_EXTI_CLEAR_IT(TOUCH_INT_PIN); // 清除????标志
    HAL_GPIO_EXTI_IRQHandler(MPU_INT_PIN); // ???? MPU6050 ?ж?
    __HAL_GPIO_EXTI_CLEAR_IT(MPU_INT_PIN); // ????ж???
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
     switch(GPIO_Pin)
    {
        case TOUCH_INT_PIN:
            xSemaphoreGiveFromISR(TouchSemaphore, &xHigherPriorityTaskWoken); // 给触摸释放二值信号量
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // 如果需要切换任务，进行上下文切换
            break;

        case MPU_INT_PIN:
            xSemaphoreGiveFromISR(MpuSemaphore, &xHigherPriorityTaskWoken); // 给 MPU6050释放二值信号量
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // 如果需要切换任务，进行上下文切换
            break;
        default : break;
    }
}

/**
 * @brief       外部中断初始化程序
 * @param       无
 * @retval      无
 */
void extix_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = TOUCH_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TOUCH_INT_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MPU_INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(MPU_INT_PORT, &GPIO_InitStruct);


    /* EXTI15_10_IRQn 是共享中断入口，触摸和 MPU6050 共用同一个优先级 */
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0); // 设置外部中断优先级
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); // 使能外部中断
}















