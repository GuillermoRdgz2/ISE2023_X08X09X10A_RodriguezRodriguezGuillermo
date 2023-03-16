#ifndef _OS_TIMER_ALARMS
#define _OS_TIMER_ALARMS

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

osTimerId_t timer_id;

uint32_t exec;

void Timer_Callback(void const *arg);
void Init_Timer(void);

#endif