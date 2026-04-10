#include "./BSP/DMA/dma.h"
#include "./SYSTEM/delay/delay.h"

DMA_HandleTypeDef  g_dma2_handle;                  /* DMA句柄 */

void dma2_init()
{
    //使能DMA2的时钟
    __HAL_RCC_DMA2_CLK_ENABLE();

    g_dma2_handle.Instance = DMA2_Stream3;                    /* 数据流选择 */
    g_dma2_handle.Init.Channel = DMA_CHANNEL_3;               /* DMA通道选择 */
    g_dma2_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;      /* 存储器到外设 */
    g_dma2_handle.Init.PeriphInc = DMA_PINC_DISABLE;          /* 外设非增量模式 */
    g_dma2_handle.Init.MemInc = DMA_MINC_ENABLE;              /* 存储器增量模式 */
    g_dma2_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; /* 外设数据长度:16位 */
    g_dma2_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD; /* 存储器数据长度:16位 */
    g_dma2_handle.Init.Mode = DMA_NORMAL;                      /* 普通模式 */

    g_dma2_handle.Init.Priority = DMA_PRIORITY_LOW;           /* 低优先级 */
    g_dma2_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* 关闭FIFO模式 */
    HAL_DMA_DeInit(&g_dma2_handle);
    HAL_DMA_Init(&g_dma2_handle);
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);//设置中断优先级
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);//使能DMA2_Stream3中断
 
}

// DMA2_Stream3中断服务函数
void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma2_handle);
}   

