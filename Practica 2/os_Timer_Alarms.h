#ifndef _OS_TIMER_ALARMS
#define _OS_TIMER_ALARMS

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

#define ALARM_ON 		0x01
#define ALARM_OFF		0x02

void Timer_Callback(void const *arg);
void Init_Timer(void);

#endif
