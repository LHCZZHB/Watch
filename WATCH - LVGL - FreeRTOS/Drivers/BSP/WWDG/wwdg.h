#ifndef __WWDG_H
#define __WWDG_H

#include "./SYSTEM/sys/sys.h"

void wwdg_init(uint8_t tr, uint8_t wr, uint32_t fprer); /*窗口看门狗初始化*/
void wwdg_set_counter(uint8_t cnt);                     /*喂窗口看门狗*/


#endif 

