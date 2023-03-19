#include "os_Timer_Alarms.h"
#include "Board_Led.h"

int cnt = 0;
osTimerId_t timerAlarm_id;
uint32_t exec_timerAlarm;

void Timer_Callback(void const *arg)
{
	if(cnt % 2 == 0){
		LED_On(2);
	}else{
		LED_Off(2);
	}
  if(cnt < 9 ){
    cnt++;
  }else{
    cnt = 0;
    osTimerStop(timerAlarm_id);
  } 
}

void Init_Timer(void)
{
	
  exec_timerAlarm = 1U;
	
  timerAlarm_id = osTimerNew((osTimerFunc_t)&Timer_Callback, osTimerPeriodic, &exec_timerAlarm,NULL);
  
}
