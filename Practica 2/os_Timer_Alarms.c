#include "os_Timer_Alarms.h"

int cnt = 0;

void Timer_Callback(void const *arg)
{
  HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_0);
  if(cnt < 9 ){
    cnt++;
  }else{
    cnt = 0;
    osTimerStop(timer_id);
  }
}

void Init_Timer(void)
{
  exec = 1U;
  timer_id = osTimerNew((osTimerFunc_t)&Timer_Callback, osTimerPeriodic, &exec,NULL);
  osTimerStart(timer_id,500U);
}