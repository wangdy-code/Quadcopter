#ifndef __LED_H
#define __LED_H
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"

typedef struct
{
	uint16_t FlashTime;
	enum
	{
		AlwaysOn, 
		AlwaysOff, 
		AllFlashLight,
		AlternateFlash, 
		WARNING,
		DANGEROURS,
		GET_OFFSET	
	}status; 
}sLED;	

extern sLED LED;
void LEDInit();
void PoletLED();
#endif // !__LED_H