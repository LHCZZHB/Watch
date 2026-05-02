#include "lvgl_demo.h"
#include "lvgl.h"
#include "./BSP/LED/led.h"

static lv_obj_t *obj_led0;                        /* 定义父对象 */
static lv_obj_t *obj_led1;                         /* 定义子对象 */

/**
 * @brief  基础对象事件回调
 * @param  *e ：事件相关参数的集合，它包含了该事件的所有数据
 * @return 无
 */
static void obj_event_cb(lv_event_t *e)
{
    lv_obj_t * target = lv_event_get_target(e);                                 /* 获取事件触发源 */

    if (target == obj_led0)                                                   /* 判断触发源：是不是父对象？ */
    {
        LED0_TOGGLE();                        /* 重新设置父对象的位置：居中 */
    
    }
    else if (target == obj_led1)                                               /* 判断触发源：是不是子对象？ */
    {
        LED1_TOGGLE();                    /* 重新设置子对象的位置：右侧居中，再向X轴偏移100 */
    }
}

void lvgl_demo(void)
{
    /* 父对象 */
    obj_led0 = lv_obj_create(lv_scr_act());                                   /* 创建父对象 */
    lv_obj_set_size(obj_led0, 50, 50);                                        /* 设置父对象的大小 */
    lv_obj_align(obj_led0, LV_ALIGN_LEFT_MID, 45, 0);                         /* 设置父对象的位置：顶部居中 */
    lv_obj_set_style_bg_color(obj_led0, lv_color_hex(0xFF0000), 0);           /* 设置父对象的背景色：红色 */
    lv_obj_add_event_cb(obj_led0, obj_event_cb, LV_EVENT_CLICKED, NULL);      /* 为父对象添加事件：按下释放后触发 */

    /* 子对象 */
    obj_led1 = lv_obj_create(lv_scr_act());                                    /* 创建子对象 */
    lv_obj_set_size(obj_led1, 50, 50);                                         /* 设置子对象的大小 */
    lv_obj_align(obj_led1, LV_ALIGN_RIGHT_MID, -45, 0);                        /* 设置子对象的位置：居中 */
    lv_obj_set_style_bg_color(obj_led1, lv_color_hex(0x73C991), 0);            /* 设置子对象的背景色：绿色 */
    lv_obj_add_event_cb(obj_led1, obj_event_cb, LV_EVENT_CLICKED, NULL);       /* 为子对象添加事件：按下释放后触发 */
}
