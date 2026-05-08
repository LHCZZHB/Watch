#include "lvgl_demo.h"
#include "lvgl.h"
#include "./BSP/LED/led.h"
#include "FreeRTOS.h"
#include "task.h"

extern unsigned long steps;
extern unsigned long walk_time;

static lv_obj_t *main_scr;
static lv_obj_t *sub_scr;
static lv_obj_t *steps_label;
static lv_obj_t *time_label;
static lv_obj_t *clock_label;

static void switch_to_sub_scr(lv_event_t *e)
{
    lv_scr_load(sub_scr);
}

static void switch_to_main_scr(lv_event_t *e)
{
    lv_scr_load(main_scr);
}

static void refresh_sub_scr(lv_timer_t *timer)
{
    unsigned long total_sec, hours, min, sec;

    /* Real-time clock from system uptime (1 tick = 1ms) */
    total_sec = xTaskGetTickCount() / 1000;
    hours = total_sec / 3600;
    min = (total_sec % 3600) / 60;
    sec = total_sec % 60;
    lv_label_set_text_fmt(clock_label, "Clock: %02lu:%02lu:%02lu", hours, min, sec);

    /* Steps counter */
    lv_label_set_text_fmt(steps_label, "Steps: %lu", steps);

    /* Walk time (accumulated walking duration) */
    total_sec = walk_time / 1000;
    hours = total_sec / 3600;
    min = (total_sec % 3600) / 60;
    sec = total_sec % 60;
    lv_label_set_text_fmt(time_label, "Walk: %02lu:%02lu:%02lu", hours, min, sec);
}

static void create_main_scr(void)
{
    main_scr = lv_obj_create(NULL);     

    lv_obj_t *icon_btn = lv_btn_create(main_scr);    
    lv_obj_set_size(icon_btn, 80, 80);      
    lv_obj_center(icon_btn);      
    lv_obj_add_event_cb(icon_btn, switch_to_sub_scr, LV_EVENT_CLICKED, NULL);

    lv_obj_t *icon_label = lv_label_create(icon_btn);
    lv_label_set_text(icon_label, LV_SYMBOL_SETTINGS);
    lv_obj_center(icon_label);
}

static void create_sub_scr(void)
{
    sub_scr = lv_obj_create(NULL);

    lv_obj_t *back_btn = lv_btn_create(sub_scr);
    lv_obj_set_size(back_btn, 60, 30);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(back_btn, switch_to_main_scr, LV_EVENT_CLICKED, NULL);

    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT " Back");
    lv_obj_center(back_label);

    clock_label = lv_label_create(sub_scr);
    lv_obj_align(clock_label, LV_ALIGN_CENTER, 0, -40);
    lv_label_set_text(clock_label, "Clock: 00:00:00");

    steps_label = lv_label_create(sub_scr);
    lv_obj_align(steps_label, LV_ALIGN_CENTER, 0, -5);
    lv_label_set_text(steps_label, "Steps: 0");

    time_label = lv_label_create(sub_scr);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 30);
    lv_label_set_text(time_label, "Walk: 00:00:00");

    lv_timer_create(refresh_sub_scr, 200, NULL);
}

void lvgl_demo(void)
{
    create_main_scr();
    create_sub_scr();
    lv_scr_load(main_scr);
}
