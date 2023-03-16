
#include "rtc.h"

/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;
RTC_AlarmTypeDef salarmstructure = {0};

/* Buffers used for displaying Time and Date */
uint8_t aShowTime[50] = {0};
uint8_t aShowDate[50] = {0};


/**
  * @brief  Configure the RTC peripheral
  * @param  None
  * @retval None
  */
void RTC_PeripheralConfig(void)
{
  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */ 
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }
  
  /*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    RTC_CalendarConfig();
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {
      /* Turn on LED2: Power on reset occurred */
      //BSP_LED_On(LED2);
    }
    /* Check if Pin Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
      /* Turn on LED1: External reset occurred */
      //BSP_LED_On(LED1);
    }
    /* Clear source Reset Flag */
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }
  
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
void RTC_CalendarConfig(void)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date */
  sdatestructure.Year = 0x23;
  sdatestructure.Month = RTC_MONTH_MARCH;
  sdatestructure.Date = 0x16;
  sdatestructure.WeekDay = RTC_WEEKDAY_THURSDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 0x00;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}

/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showtime, "%2d:%2d:%2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : mm-dd-yy */
  sprintf((char *)showdate, "%2d-%2d-%2d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
  EscribeFrase((char *)showtime,0);
  EscribeFrase((char *)showdate,1);
}

/**
  * @brief  Set an alarm at and specified time that generates an interruption
  * @param  hour: hour of the alarm in BCD format
  * @param  minutes: minutes of the alarm in BCD format
  * @param  seconds: seconds of the alarm in BCD format
  * @retval None
  */
void RTC_SetAlarm(uint8_t hour, uint8_t minutes, uint8_t seconds)
{
  
  salarmstructure.AlarmTime.Hours = hour;
  salarmstructure.AlarmTime.Minutes = minutes;
  salarmstructure.AlarmTime.Seconds = seconds;
  
  salarmstructure.Alarm = RTC_ALARM_A;
  
  salarmstructure.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
  
  salarmstructure.AlarmTime.StoreOperation = RTC_STOREOPERATION_SET;
  
  HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure,RTC_FORMAT_BCD);  
}

void RTC_SetMinutesAlarm(void)
{
  
  salarmstructure.AlarmTime.Seconds = 0x00;
  salarmstructure.AlarmMask = RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES | RTC_ALARMMASK_DATEWEEKDAY;
  
  salarmstructure.Alarm = RTC_ALARM_A;
  
  salarmstructure.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
  
  salarmstructure.AlarmTime.StoreOperation = RTC_STOREOPERATION_SET;
  
  HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure,RTC_FORMAT_BCD);
}
