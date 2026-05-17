#include "FreeRTOS.h"
#include "task.h"
#include "freertos_demo.h"
#include "semphr.h"


#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lvgl_demo.h"
#include "./SYSTEM/usart/usart.h"


#include "./BSP/LED/led.h"
#include "./BSP/MPU6050/mpu6050.h"
#include "inv_mpu.h"

/******************************************************************************************************/
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO     1           /* 任务优先级 */
#define START_STK_SIZE      128         /* 任务堆栈大小 */
TaskHandle_t StartTask_Handler;         /* 任务句柄 */
void start_task(void *pvParameters);    /* 任务函数 */

/* LV_DEMO_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define LV_DEMO_TASK_PRIO   3           /* 任务优先级 */
#define LV_DEMO_STK_SIZE    1024        /* 任务堆栈大小 */
TaskHandle_t LV_DEMOTask_Handler;       /* 任务句柄 */
void lv_demo_task(void *pvParameters);  /* 任务函数 */

/* MPU6050任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define MPU6050_TASK_PRIO   2           /* 任务优先级 */
#define MPU6050_STK_SIZE    512         /* 任务堆栈大小 */
TaskHandle_t MPU6050Task_Handler;       /* 任务句柄 */
void mpu6050_task(void *pvParameters);      /* 任务函数 */

SemaphoreHandle_t MpuSemaphore = NULL;             /* MPU6050数据访问二值信号量 */  
SemaphoreHandle_t TouchSemaphore = NULL;           /* 触摸数据访问二值信号量 */
SemaphoreHandle_t MutexSemaphore = NULL;           /* 互斥信号量  */

unsigned long steps = 0;
unsigned long walk_time = 0;
/******************************************************************************************************/


/**
 * @brief       freertos_demo入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    lv_init();                                          /* lvgl系统初始化 */
    lv_port_disp_init();                                /* lvgl显示接口初始化,放在lv_init()的后面 */
    lv_port_indev_init();                               /* lvgl输入接口初始化,放在lv_init()的后面 */

    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传递给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */

    vTaskStartScheduler();                              /* 开启任务调度 */
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    pvParameters = pvParameters;
    
    taskENTER_CRITICAL();           /* 进入临界区 */

    //创建二值信号量
    MpuSemaphore = xSemaphoreCreateBinary();
    TouchSemaphore = xSemaphoreCreateBinary();
    /* 创建互斥信号量 */
    MutexSemaphore = xSemaphoreCreateMutex();
    
    /* 创建LVGL任务 */
    xTaskCreate((TaskFunction_t )lv_demo_task,
                (const char*    )"lv_demo_task",
                (uint16_t       )LV_DEMO_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LV_DEMO_TASK_PRIO,
                (TaskHandle_t*  )&LV_DEMOTask_Handler);

    /* MPU6050任务 */
    xTaskCreate((TaskFunction_t )mpu6050_task,
                (const char*    )"mpu6050_task",
                (uint16_t       )MPU6050_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )MPU6050_TASK_PRIO,
                (TaskHandle_t*  )&MPU6050Task_Handler);

    taskEXIT_CRITICAL();            /* 退出临界区 */
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
}

/**
 * @brief       LVGL运行例程
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void lv_demo_task(void *pvParameters)
{
    pvParameters = pvParameters;
    uint16_t x, y;
    uint8_t gesture,finger_num;
    
    lvgl_demo();
    
    while(1)
    {
        if(xSemaphoreTake(TouchSemaphore, 0) == pdTRUE) 
        {
            xSemaphoreTake(MutexSemaphore, portMAX_DELAY);  /* 获取互斥信号量 */
            cst816t_getaction(&x, &y, &gesture, &finger_num); // 获取触摸状态
            xSemaphoreGive(MutexSemaphore);                 /* 释放互斥信号量 */
        }                    /* LVGL 任务处理函数 */
        
        lv_timer_handler(); /* LVGL计时器 */
        vTaskDelay(5);
    }
}

/**
 * @brief       mpu6050_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void mpu6050_task(void *pvParameters)
{
    pvParameters = pvParameters;
    //打印mpu_itr状态
    // int16_t accel_data[3];
    // int16_t gyro_data[3];
    float pitch = 0.0f;
    float roll = 0.0f;
    float yaw = 0.0f;
    uint8_t int_status;
    uint8_t dmp_ret;


    xSemaphoreTake(MutexSemaphore, portMAX_DELAY);  /* 获取互斥信号量 */
    mpu6050_dmp_init(); /* MPU6050初始化 */
    xSemaphoreGive(MutexSemaphore);                 /* 释放互斥信号量 */

    while(1)
    { 
        if(xSemaphoreTake(MpuSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) 
        {
            xSemaphoreTake(MutexSemaphore, portMAX_DELAY);  /* 获取互斥信号量 */
            
            int_status = mpu6050_receivebyte(INT_STATUS);
            dmp_ret = mpu6050_readDMP(&pitch, &roll, &yaw); // 获取DMP数据并计算欧拉角
            if(dmp_ret != 0)
            {
                mpu_reset_fifo();
                printf("DMP read failed: %u, INT_STATUS=0x%02X\r\n", dmp_ret, int_status);
            }

            // 获取计步器数据
            mpu6050_get_steps(&steps, &walk_time);
            // mpu6050_readDMP(accel_data, gyro_data); // 获取MPU6050数据
            xSemaphoreGive(MutexSemaphore);                 /* 释放互斥信号量 */
            printf("Steps: %lu, Walk Time: %lu ms\r\n", steps, walk_time);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}


