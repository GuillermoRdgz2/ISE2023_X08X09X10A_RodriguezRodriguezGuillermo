
#ifndef __LCD_H
#define __LCD_H

#include "stdio.h"
#include "string.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"


void Delay(uint32_t n_microsegundos);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void LCD_init(void);
void LCD_update(void);
void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
void LCD_symbolToLocalBuffer_L2(uint8_t symbol);
void symbolToLocalBuffer(uint8_t line,uint8_t symbol);
void LCD_Reset(void);
void SPI_Init(void);
void EscribeFrase(char *frase, uint8_t linea);
void LCD_Clean(void);
void Examen_BT2(void);
void barrido_arriba_abajo(void);
void barrido_izq_der(void);
void Examen_BT2_Posible_2(void);
void izquierda(void);
void derecha(void);
void arriba(void);
void abajo(void);
void pintar_pelota(void);
//void BarridoVertical(void);
//void BarridoHorizontal(int linea);
//void DesplazaLetra(void);
//void DesplazaLetra2(void);
//void DesplazarLetraSeleccionada(uint8_t line);
//void prueba(void);
//void Cuadrados(void);
#endif
