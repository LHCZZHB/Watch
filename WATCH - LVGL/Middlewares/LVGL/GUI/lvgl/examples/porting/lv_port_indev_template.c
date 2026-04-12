/**
 * @file lv_port_indev_templ.c
 *
 */

 /*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev_template.h"
#include "../../lvgl.h"
#include "./BSP/TOUCH/cst816.h"
#include "./SYSTEM/usart/usart.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;
static uint8_t last_finger_num = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{

    static lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);

}

/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    cst816t_init();
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;
    uint16_t touch_x = 0;
    uint16_t touch_y = 0;
    uint8_t gesture = 0;
    uint8_t finger_num = 0;

    cst816t_getaction(&touch_x, &touch_y, &gesture, &finger_num);
    last_finger_num = finger_num;

    /*Save the pressed coordinates and the state*/
    if(touchpad_is_pressed()) {
        last_x = (lv_coord_t)touch_x;
        last_y = (lv_coord_t)touch_y;
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Set the last pressed coordinates*/
    data->point.x = last_x;
    data->point.y = last_y;
}

/*Return true if the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    return (last_finger_num != 0);
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    uint16_t touch_x = 0;
    uint16_t touch_y = 0;
    uint8_t gesture = 0;
    uint8_t finger_num = 0;

    cst816t_getaction(&touch_x, &touch_y, &gesture, &finger_num);
    last_finger_num = finger_num;
    (*x) = (lv_coord_t)touch_x;
    (*y) = (lv_coord_t)touch_y;
    printf("Touchpad X: %d, Y: %d\r\n", *x, *y);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
