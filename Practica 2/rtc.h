#ifndef _RTC_H
#define _RTC_H

#include "main.h"
#include "lcd.h"
#include <stdio.h>

#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

/* RTC function prototypes ---------------------------------------------------*/
void RTC_CalendarConfig(void);
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate);

void RTC_PeripheralConfig(void);

void RTC_SetAlarm(uint8_t hour, uint8_t minutes, uint8_t seconds);
void RTC_SetMinutesAlarm(void);



#endif
