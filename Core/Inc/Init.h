#ifndef __INIT_H
#define __INIT_H
#include "stm32f1xx_hal.h"

extern volatile uint32_t ST_CpuID;

void Init();
#endif // !__INIT_H