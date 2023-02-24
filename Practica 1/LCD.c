#include "LCD.h"
#include "main.h"
#include "Arial12x12.h"
#include <stdio.h>
#include <string.h>
#include "cmsis_os2.h"  
#include "stm32f4xx_hal.h"

#define LINEA_PREDETERMINADA_ESCRITURA 1
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
//osThreadId_t tid_ThLCD;                        // thread id
//osMessageQueueId_t mid_LCDQueue;
//void ThLCD (void *argument);                   // thread function
MENSAJE_LCD_t textoLCD;


/******************************************************************************/
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
TIM_HandleTypeDef htim7;
GPIO_InitTypeDef GPIO_InitStruct;

uint16_t posicionL1 = 4;
uint16_t posicionL2 = 4;
char buffer[512];

void LCD_reset(void){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	//Configuración de los pines*****************************************************
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	//RESET PA6
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	
	//A0 PF13
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);

	
	//CS_N PD14
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	
	/*******************************************************************************/
	
	//Inicialización del SPI_Driver
	SPIdrv->Initialize(NULL);
	//Encendemos el SPI
	SPIdrv->PowerControl(ARM_POWER_FULL);
	//Configuración del SPI_Driver
	SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8) , 20000000);
	
	//Reset inicial
	reset_SPI();
	delay(1000);
}

void reset_SPI(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	delay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
}


void delay(uint32_t n_microsegundos){
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = 20;
  htim7.Init.Period = (n_microsegundos/4)-1;
	
	__HAL_RCC_TIM7_CLK_ENABLE();
	
	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start_IT(&htim7);
	
	while(TIM7 -> CNT){};
		
		HAL_TIM_Base_DeInit(&htim7);
		HAL_TIM_Base_Stop(&htim7);
}



void LCD_wr_data(unsigned char data){
	ARM_SPI_STATUS estado;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);		//CS = 0
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);		//A0 = 1 (TRANSMISION DATOS)
	SPIdrv->Send(&data,sizeof(data));
	while(estado.busy){
		estado = SPIdrv->GetStatus();
	}
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);		//CS = 1
 
}

void LCD_wr_cmd(unsigned char cmd){
	ARM_SPI_STATUS estado;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);		//CS = 0
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);		////A0 = 0 (TRANSMISION COMANDOS)
  SPIdrv->Send(&cmd,sizeof(cmd));
	while(estado.busy){
		estado = SPIdrv->GetStatus();
	}
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);		//CS = 1
}


void LCD_init(void){
	LCD_wr_cmd(0xAE);	
	LCD_wr_cmd(0xA2);		
	LCD_wr_cmd(0xA0);		
	LCD_wr_cmd(0xC8);		
	LCD_wr_cmd(0x22);	
	LCD_wr_cmd(0x2F);		
	LCD_wr_cmd(0x40);		
	LCD_wr_cmd(0xAF);		
	LCD_wr_cmd(0x81);		
  LCD_wr_cmd(0x15);   
	LCD_wr_cmd(0xA4);		
	LCD_wr_cmd(0xA6);
}

void LCD_update(void){
	int i;
	LCD_wr_cmd(0x00);		//4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10);		//4 bits de la parte alta de la dirección a 0	
	LCD_wr_cmd(0xB0);		//Página 0
	
	for(i = 0 ; i < 128 ; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB1);		//Página 1
	
	for(i = 128 ; i < 256 ; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB2);		//Página 2
	
	for(i = 256 ; i < 384 ; i++){
		LCD_wr_data(buffer[i]);
	}

	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB3);		//Página 3
	
	for(i = 384 ; i < 512 ; i++){
		LCD_wr_data(buffer[i]);
	}
}

void EscribeLetra_L1(uint8_t letra){
  uint8_t i, valor1, valor2;
  uint16_t offset = 0;
  
  offset = 25 * (letra - ' ');
  
  for (i = 0; i<12; i++){
     valor1 = Arial12x12 [offset+i*2+1];
     valor2 = Arial12x12 [offset+i*2+2];  

     buffer [i+posicionL1] = valor1;
     buffer [i+128+posicionL1] = valor2;
    LCD_update();
  }
  posicionL1 = posicionL1 + Arial12x12 [offset];
  
  if ( posicionL1 > 127)
    posicionL1 = 4;
  
  LCD_update(); 
}

void EscribeLetra_L2(uint8_t letra){
  uint8_t i, valor1, valor2;
  uint16_t offset = 0;
  
  offset = 25 * (letra - ' ');
  
  for (i = 0; i<12; i++){
     valor1 = Arial12x12 [offset+i*2+1];
     valor2 = Arial12x12 [offset+i*2+2];  

     buffer [i+256+posicionL2] = valor1;
     buffer [i+384+posicionL2] = valor2;
  }
  posicionL2 = posicionL2 + Arial12x12 [offset];
  
  if ( posicionL2 > 127)
    posicionL2 = 4;
  
  LCD_update();
}

void Escribir_Frase(int linea, char frase[]){
	uint8_t i,j, longitud;
	posicionL1 = 4;
	posicionL2 = 4;
	
	longitud = strlen(frase);
	
	if(linea == 1){
		for(i = 0; i < longitud ; i++){
			EscribeLetra_L1(frase[i]);
		} 
	}else{
		for(j = 0; j < longitud ; j++){
			EscribeLetra_L2(frase[j]);
		} 
	}
}

void encender_LCD(void){
	LCD_wr_cmd(0xAF);
}

void apagar_LCD(void){
	LCD_wr_cmd(0xAE);
}

void negativo_LCD(void){
	LCD_wr_cmd(0xA7);
}

void positivo_LCD(void){
	LCD_wr_cmd(0xA6);
}

void pintarCuadrado(int posicion){
	int ancho = 8;
	
	for(int i = 0 ; i < ancho ; i++){
		buffer[i + posicion] = 0xFF;
		buffer[i + 128 + posicion] = 0x0F;
	}
	
	LCD_update();
}

void pintarSimboloPosicion(uint8_t simbolo,int posicion,int linea){
	uint8_t i, valor1, valor2;
  uint16_t offset = 0;
  
  offset = 25 * (simbolo - ' ');
  
  for (i = 0; i<12; i++){
		valor1 = Arial12x12 [offset+i*2+1];
		valor2 = Arial12x12 [offset+i*2+2];  
		
		if(linea == 1){
			buffer [i+posicionL1] = valor1;
			buffer [i+128+posicionL1] = valor2;
		}else{
			buffer [i+256+posicionL2] = valor1;
			buffer [i+384+posicionL2] = valor2;
		}
		
    LCD_update();
  }
  posicionL1 = posicionL1 + Arial12x12 [offset];
  
  if ( posicionL1 > 127)
    posicionL1 = 4;
  
  LCD_update(); 
}

void limpiar_LCD(void){
	for(int i = 0 ; i < 512 ; i++){
		buffer[i] = 0x00;
	}
	LCD_update();
}
void SPI_init(void){
	//Inicialización del SPI_Driver
	SPIdrv->Initialize(NULL);
	//Encendemos el SPI
	SPIdrv->PowerControl(ARM_POWER_FULL);
	//Configuración del SPI_Driver
	SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8) , 20000000);
}
/******************************************************************************/
 
//int Init_ThLCD (void) {
// 
//  tid_ThLCD = osThreadNew(ThLCD, NULL, NULL);
//  if (tid_ThLCD == NULL) {
//    return(-1);
//  }
// 
//  return(0);
//}

//int Init_LCDQueue(void){
//	mid_LCDQueue = osMessageQueueNew(16,sizeof(MENSAJE_LCD_t),NULL);
//	if(mid_LCDQueue != osOK){
//		return(-1);
//	}
//	return(0);
//}
// 
//void ThLCD (void *argument) {
//	osStatus_t status;
//	
//	Init_LCDQueue();
//	LCD_reset();
//	LCD_init();
//	
//	
//  while (1) {
//		
//		
//		
//    osThreadYield();                            // suspend thread
//  }
//}
