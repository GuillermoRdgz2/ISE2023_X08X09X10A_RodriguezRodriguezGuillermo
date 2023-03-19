/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "LCD.h"
#include "adc.h"
#include "rtc.h"
#include "os_Timer_Alarms.h"

extern void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate);
extern uint8_t aShowTime[50];
extern uint8_t aShowDate[50];

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};


//extern uint16_t AD_in          (uint32_t ch);
extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];

extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;
extern osThreadId_t TID_RTC_Time;
extern osTimerId_t timerAlarm_id;

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };

ADC_HandleTypeDef adchandle;
int32_t val = 0;
float voltage = 0;

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_RTC_Time;

/* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);
static void RTC_Time (void *arg);

__NO_RETURN void app_main (void *arg);

/* Read analog inputs */
uint32_t AD_in (uint32_t ch) {
  //int32_t val = 0;
	

  if (ch == 0) {
		voltage = ADC_getVoltage(&adchandle , 10 ); //get values from channel 10->ADC123_IN10
		
		//printf("Voltage: %d",val);
  }
  return ((uint32_t)voltage);
}

/* Read digital inputs */
uint8_t get_button (void) {
  return ((uint8_t)Buttons_GetState ());
}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Display (void *arg) {

  (void)arg;

  while(1) {
		RTC_CalendarShow(aShowTime,aShowDate);
		Delay(250);
  }
}

/*----------------------------------------------------------------------------
  Thread 'RTC': RTC and alarm handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void RTC_Time (void *arg) {

  while(1) {
		osThreadFlagsWait(ALARM_ON,osFlagsWaitAny,osWaitForever);
		osThreadFlagsClear(ALARM_ON);
		osTimerStart(timerAlarm_id,500U);
	}
}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  uint32_t cnt = 0U;

  (void)arg;

  LEDrun = true;
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      LED_SetOut (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

  LED_Initialize();
  Buttons_Initialize();
	LCD_Reset();
	LCD_init();
	LCD_Clean();
	LCD_update();
	
	RTC_PeripheralConfig();
	Init_Timer();
	RTC_CalendarConfig();
	RTC_SetMinutesAlarm();
 
	ADC1_pins_F429ZI_config();
	ADC_Init_Single_Conversion(&adchandle , ADC1); //ADC1 configuration

  netInitialize ();

  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);
	TID_RTC_Time= osThreadNew (RTC_Time, NULL, NULL);

  osThreadExit();
}
