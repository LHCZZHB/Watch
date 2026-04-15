#include "./BSP/SPI/spi.h"
#include "./BSP/DMA/dma.h"

SPI_HandleTypeDef g_spi1_handle;//SPI句柄
extern DMA_HandleTypeDef g_dma2_handle;//DMA句柄

/**
 * @brief       SPI初始化代码
 *   @note      主机模式,8位数据,禁止硬件片选
 * @param       无
 * @retval      无
 */
 void spi1_init(void)
 {
    __HAL_RCC_SPI1_CLK_ENABLE();//使能SPI1时钟

    g_spi1_handle.Instance = SPI1;//SPI1
    g_spi1_handle.Init.Mode = SPI_MODE_MASTER;//主机模式
    g_spi1_handle.Init.Direction = SPI_DIRECTION_2LINES;//双线模式
    g_spi1_handle.Init.DataSize = SPI_DATASIZE_8BIT;//8位数据
    g_spi1_handle.Init.CLKPolarity = SPI_POLARITY_HIGH;//时钟极性
    g_spi1_handle.Init.CLKPhase = SPI_PHASE_2EDGE;//时钟相位
    g_spi1_handle.Init.NSS = SPI_NSS_SOFT;//软件片选
    g_spi1_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;//分频系数
    g_spi1_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;//MSB优先
    g_spi1_handle.Init.TIMode = SPI_TIMODE_DISABLE;//禁用时间戳
    g_spi1_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;//禁用CRC校验
    g_spi1_handle.Init.CRCPolynomial = 7;//CRC多项式    
     
    HAL_SPI_Init(&g_spi1_handle);//初始化SPI1
     
    __HAL_SPI_ENABLE(&g_spi1_handle);
     
    
 }

 /**
 * @brief       SPI1底层驱动，时钟使能，引脚配置
 *   @note      此函数会被HAL_SPI_Init()调用
 * @param       hspi:SPI句柄
 * @retval      无
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    if(hspi->Instance==SPI1)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();//使能GPIOB时钟

        //SPI1 SCK PB3引脚模式设置（复用输出）
        gpio_init_struct.Pin = GPIO_PIN_3;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;//复用推挽输出
        gpio_init_struct.Pull = GPIO_PULLUP;//无上下拉
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;//高速
        gpio_init_struct.Alternate = GPIO_AF5_SPI1;//复用为SPI1
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);

        //SPI1 MISO PB4引脚模式设置（复用输入）
        gpio_init_struct.Pin = GPIO_PIN_4;
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);

        //SPI1 MOSI PB5引脚模式设置（复用输出）
        gpio_init_struct.Pin = GPIO_PIN_5;
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);
    
        __HAL_LINKDMA(hspi, hdmatx, g_dma2_handle); //将SPI1的发送DMA2联系起来
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {

    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PB3     ------> SPI1_SCK
    PB5     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_5);
    HAL_DMA_DeInit(spiHandle->hdmatx);
}
}
/**
 * @brief       SPI1速度设置函数
 *   @note      SPI1时钟选择来自APB2, 即PCLK2, 为 84MHz
 *              SPI速度 = PCLK2 / 2^(speed + 1)
 * @param       speed   : SPI1时钟分频系数
                        取值为SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
 * @retval      无
 */
void spi1_set_speed(uint8_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed)); /* 判断有效性 */
    __HAL_SPI_DISABLE(&g_spi1_handle);             /* 关闭SPI */
    g_spi1_handle.Instance->CR1 &= 0XFFC7;         /* 位3-5清零，用来设置波特率 */
    g_spi1_handle.Instance->CR1 |= speed << 3;     /* 设置SPI速度 */
    __HAL_SPI_ENABLE(&g_spi1_handle);              /* 使能SPI */
}

/**
 * @brief       SPI1读写一个字节数据
 * @param       txdata  : 要发送的数据(1字节)
 * @retval      接收到的数据(1字节)
 */
uint8_t spi1_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    HAL_SPI_TransmitReceive(&g_spi1_handle, &txdata, &rxdata, 1, 1000); /* 发送并接收1字节数据，超时时间1000ms */
    return rxdata;
}

