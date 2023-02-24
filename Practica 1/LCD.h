#ifndef _THLCD_H
#define _THLCD_H

#include "Driver_SPI.h"
#include "cmsis_os2.h"

extern ARM_DRIVER_SPI Driver_SPI1;

//CONSTANTES

//FUNCIONES
void delay(uint32_t n_microsegundos);
void LCD_reset(void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void LCD_init(void);
void LCD_update(void);
void reset_SPI(void);
void EscribeLetra_L1(uint8_t letra);
void EscribeLetra_L2(uint8_t letra);
void Escribir_Frase(int linea, char frase[]);
void encender_LCD(void);
void apagar_LCD(void);
void negativo_LCD(void);
void positivo_LCD(void);
void pintarCuadrado(int posicion);
void pintarSimboloPosicion(uint8_t simbolo,int posicion,int linea);
void limpiar_LCD(void);
void SPI_init(void);

//extern int Init_ThLCD (void);
//extern int Init_LCDQueue(void);

//VARIABLES
typedef struct{
	int linea;
	char texto[255];
} MENSAJE_LCD_t;

//extern osThreadId_t tid_ThLCD;
//extern osMessageQueueId_t mid_LCDQueue;

#endif
