#include "Arial12x12.h"
#include "lcd.h"

//Variables globales
  extern ARM_DRIVER_SPI Driver_SPI1;	    //Configuracion del Driver en el SP1
  ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;	//Configuracion del Driver en el SP1
	
	TIM_HandleTypeDef tim7;
GPIO_InitTypeDef GPIO_InitStruct;
	
	unsigned char buffer[512];
  uint16_t positionL1 = 0;
	uint16_t positionL2 = 256;
	int i;


//TIMER7 DELAY
void Delay(uint32_t n_microsegundos){
	
	tim7.Instance = TIM7;					//Esta linea instancia el timer
	tim7.Init.Prescaler = 20;     //84/21 = 4 MHz
	tim7.Init.Period = (n_microsegundos/4)-1; //Pone el periodo	
	
	__HAL_RCC_TIM7_CLK_ENABLE();		//Encendemos el periferico TIM7 HAL
	
	//Esperamos a que se active el FLAG del MATCH
	
	HAL_TIM_Base_Init(&tim7);			//Configura el timer
	HAL_TIM_Base_Start_IT(&tim7);	//Inicializa el timer

	while(TIM7 -> CNT){};
		
	HAL_TIM_Base_DeInit(&tim7);
	HAL_TIM_Base_Stop(&tim7);

	
	//EJEMPLO DE CONFIGURACION
	//Se requiere el LED con un periodo 3s(1.5S apagado y 1.5S encendido)
	//Lo resolveremos sabiendo que el TIMER7 esta conectado a la APB1 segun el manual del STM32F429
	//Entonces nos entran 84 MHz / Prescaler = 84 000 = 10 kHz 
	//Como queremos que el timer salte cada 1.5S entonces Period = 1.5S / 10kHz = 14999

		
}


//CONFIGURACION DEL SPI
void LCD_Reset(void){ 
	
	
/*o Modo master (ARM_SPI_MODE_MASTER)
	o CPOL1 y CPHA1 (ARM_SPI_CPOL1_CPHA1)
	o Organización de la información MSB a LSB (ARM_SPI_MSB_LSB)
	o 8 bits de datos (ARM_SPI_DATA_BITS(8))
	o Frecuencia del SCLK, 20MHz*/
	
	SPIdrv->Initialize(NULL);
	SPIdrv->PowerControl(ARM_POWER_FULL);
	SPIdrv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_CPOL1_CPHA1|ARM_SPI_MSB_LSB|ARM_SPI_SS_MASTER_UNUSED|ARM_SPI_DATA_BITS(8),20000000);
	
	    //Configuramos los pines RESET,A0,CS como salida
      //Instanciamos los pines como salida.
	
	    __HAL_RCC_GPIOA_CLK_ENABLE();
		
        GPIO_InitStruct.Pin = GPIO_PIN_6;//RESET
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
    __HAL_RCC_GPIOF_CLK_ENABLE();
		
        GPIO_InitStruct.Pin = GPIO_PIN_13;//A0
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		
    __HAL_RCC_GPIOD_CLK_ENABLE();
		
        GPIO_InitStruct.Pin = GPIO_PIN_14;//CS
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
		
		//Inicialmente los pines de RESET, A0, CS :
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);//CS
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);//RESET
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);//A0
		
		//USAMOS TIMER 7 PARA APAGAR PANTALLA Y ESPERAR UN TIEMPO	DE 1MS
		
	  Delay(1000); // Esperamos un milisegundo
		
    //Apagamos el lcd
		
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);//RESET
		
    Delay(1000);
		
	  //Encendemos el lcd
		
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);//RESET
	
}

void LCD_init(void){
	

	LCD_wr_cmd(0xAE);		//Display off

	LCD_wr_cmd(0xA2);	//Polarizacion lcd a 1/9

	LCD_wr_cmd(0xA0);	//Direccionamiento ram

	LCD_wr_cmd(0xC8);	//Salidas com 

	LCD_wr_cmd(0x22);	//Resistencias internas

	LCD_wr_cmd(0x2F);	//Power on

	LCD_wr_cmd(0x40);	//Display en la linea 0

	LCD_wr_cmd(0xAF);	//Display ON

	LCD_wr_cmd(0x81);	//Contraste

	LCD_wr_cmd(0x13);	//Valor contraste

	LCD_wr_cmd(0xA4);	//Display all points normal

	LCD_wr_cmd(0xA6);	//LCD display normal
	
}

void LCD_update(void){ //ACTUALIZA EL VALOR DEL DISPLAY CON EL ARRAY
	
 int j;
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB0); // Página 0

 for(j=0;j<128;j++){
 LCD_wr_data(buffer[j]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB1); // Página 1

 for(j=128;j<256;j++){
 LCD_wr_data(buffer[j]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB2); //Página 2
 for(j=256;j<384;j++){
 LCD_wr_data(buffer[j]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB3); // Pagina 3


 for(j=384;j<512;j++){
 LCD_wr_data(buffer[j]);
 }
}

void LCD_wr_cmd(unsigned char cmd){
	
	ARM_SPI_STATUS status;
	
 // Seleccionar CS = 0;
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
	
 // Seleccionar A0 = 0;

	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_RESET);
	
 // Escribir un comando (cmd) usando la función SPIDrv->Send(…);
	
	SPIdrv->Send(&cmd,sizeof(cmd));//El tamano es en BYTES
	
 // Esperar a que se libere el bus SPI;
	
				do{											 //REALIZAMOS ESTE BUCLE HASTA QUE ESTE LIBRE 
			 status = SPIdrv->GetStatus();
		   }while(status.busy);
	
			Delay(1000);  //Y esperamos 1 ms hasta que se reinicie el LCD
			 
 // Seleccionar CS = 1;
			 
			 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);
}

void LCD_wr_data(unsigned char data){
	
	ARM_SPI_STATUS status;
	
 // Seleccionar CS = 0;
	
 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);
	
 // Seleccionar A0 = 1;
	
 HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_SET);
	
 // Escribir un dato (data) usando la función SPIDrv->Send(…);
	
	SPIdrv->Send(&data,sizeof(data));//El tamano es en BYTES
	
 // Esperar a que se libere el bus SPI;
	
			 do{											 //REALIZAMOS ESTE BUCLE HASTA QUE ESTE LIBRE 
			 status = SPIdrv->GetStatus();
		   }while(status.busy);
			 
			 Delay(1000); //Y esperamos 1 ms hasta que se reinicie el LCD
			 
 // Seleccionar CS = 1;
			 
			 HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);

}


void LCD_symbolToLocalBuffer_L1(uint8_t symbol){
	

	uint8_t i, value1,value2;
	uint16_t offset = 0; //Se trata del comienzo de escritura
	
	offset=25*(symbol - ' '); //Avanza hasta la linea que se encuentre la proxima letra a representar
	//Letras Arial tienen un tamano de 25 bytes --> 1 byte una columna --> 8bits
	for (i=0; i<12; i++){  //Se escriben las primeras 12 columnas de la letra	
		
		value1 = Arial12x12[offset+i*2+1]; //Escribe columnas impares
		value2 = Arial12x12[offset+i*2+2]; //Esccribe columnas pares
		
		buffer[i + positionL1]= value1; //Pagina 0, escribe valores impares el -1 es para que comience el la posicion 0 y no se desplace
		buffer[i + 128 + positionL1]= value2;//Pagina 1 escribimos los valores pares
	}
	  positionL1=positionL1+Arial12x12[offset]; //Indica que posicion columna se debe comenzar a escribir
}

void LCD_symbolToLocalBuffer_L2(uint8_t symbol){
	

	uint8_t i, value1,value2;
	uint16_t offset = 0; //Se trata del comienzo de escritura
	
	offset=25*(symbol - ' '); //Avanza hasta la linea que se encuentre la proxima letra a representar
	//Letras Arial tienen un tamano de 25 bytes --> 1 byte una columna --> 8bits
	for (i=0; i<12; i++){  //Se escriben las primeras 12 columnas de la letra	
		
		value1 = Arial12x12[offset+i*2+1]; //Escribe columnas impares
		value2 = Arial12x12[offset+i*2+2]; //Esccribe columnas pares
		
		buffer[i + positionL2]= value1; //Pagina 0, escribe valores impares
		buffer[i + 128 + positionL2]= value2;//Pagina 1 escribimos los valores pares
	}
	  positionL2=positionL2+Arial12x12[offset]; //Indica que posicion columna se debe comenzar a escribir
}


void symbolToLocalBuffer(uint8_t line,uint8_t symbol){
	
	if(line == 0){
		LCD_symbolToLocalBuffer_L1(symbol);
	}
	
	if(line == 1){
		LCD_symbolToLocalBuffer_L2(symbol);
	
  }
}

void EscribeFrase(char *frase, uint8_t line)
{
	uint8_t i;
 
	
	if(line == 0){
		for(i=0; i<strlen(frase); i++){
			if(positionL1+12 < 127)
				LCD_symbolToLocalBuffer_L1(frase[i]);
		}
	}
	if(line == 1){
		for(i=0; i<strlen(frase); i++){
			if(positionL2+12 < 383)
				LCD_symbolToLocalBuffer_L2(frase[i]);
		}
	}
	positionL2 = 256;
	positionL1 = 0;
	LCD_update();
}

//Limpiar LCD
void LCD_Clean(void)
{
	memset(buffer,0,512);
	LCD_update();
}


//int z=0;
//void BarridoVertical(void){

//	LCD_Clean();
//			
//			buffer[z] = 0xFF;
//			
//			buffer[z+128] = 0xFF;
//			
//			buffer[z+256] = 0xFF;
//			
//			buffer[z+384] = 0xFF;
//	
//			LCD_update();
//	
//			z++;
//	
//	if (z == 128){
//		z=0;
//	}
//	
//}
//int h=0;
//void BarridoHorizontal(int linea)
//{
//	
//		LCD_Clean();
//	
//	if(linea < 9){
//		for (h=0 ; h < 128 ; h++){
//			if (linea == 1)
//				buffer[h] = 0x01;
//			
//			if (linea == 2)
//				buffer[h] = 0x02;
//			
//			if (linea == 3)
//				buffer[h] = 0x04;
//	
//			if (linea == 4)
//				buffer[h] = 0x08;
//			
//			if (linea == 5)
//				buffer[h] = 0x10;
//			
//			if (linea == 6)
//				buffer[h] = 0x20;
//			
//			if (linea == 7)
//				buffer[h] = 0x40;
//	
//			if (linea == 8)
//				buffer[h] = 0x80;
//		}
//	}
//	
//		if(linea > 8 && linea < 17){
//		for (h=128 ; h < 256 ; h++){
//			if (linea == 9)
//				buffer[h] = 0x01;
//			
//			if (linea == 10)
//				buffer[h] = 0x02;
//			
//			if (linea == 11)
//				buffer[h] = 0x04;
//	
//			if (linea == 12)
//				buffer[h] = 0x08;
//			
//			if (linea == 13)
//				buffer[h] = 0x10;
//			
//			if (linea == 14)
//				buffer[h] = 0x20;
//			
//			if (linea == 15)
//				buffer[h] = 0x40;
//	
//			if (linea == 16)
//				buffer[h] = 0x80;
//		}
//	}
//		
//			if(linea > 16 && linea < 25){
//		for (h=256 ; h < 384 ; h++){
//			if (linea == 17)
//				buffer[h] = 0x01;
//			
//			if (linea == 18)
//				buffer[h] = 0x02;
//			
//			if (linea == 19)
//				buffer[h] = 0x04;
//	
//			if (linea == 20)
//				buffer[h] = 0x08;
//			
//			if (linea == 21)
//				buffer[h] = 0x10;
//			
//			if (linea == 22)
//				buffer[h] = 0x20;
//			
//			if (linea == 23)
//				buffer[h] = 0x40;
//	
//			if (linea == 24)
//				buffer[h] = 0x80;
//		}
//	}			if(linea > 24 && linea < 33){
//		for (h=384 ; h < 512 ; h++){
//			if (linea == 25)
//				buffer[h] = 0x01;
//			
//			if (linea == 26)
//				buffer[h] = 0x02;
//			
//			if (linea == 27)
//				buffer[h] = 0x04;
//	
//			if (linea == 28)
//				buffer[h] = 0x08;
//			
//			if (linea == 29)
//				buffer[h] = 0x10;
//			
//			if (linea == 30)
//				buffer[h] = 0x20;
//			
//			if (linea == 31)
//				buffer[h] = 0x40;
//	
//			if (linea == 32)
//				buffer[h] = 0x80;
//		}
//	}
//				LCD_update();
//}


//int pos1 = 0;

//void DesplazaLetra(void){

//      LCD_Clean();
//	
//      if(positionL1<127){   //MAXIMO DE LA PAGINA1 
//        positionL1=pos1;
//        LCD_symbolToLocalBuffer_L1('@');
//        }
//			
//				 LCD_update();
//				 pos1++;
//				
//				if (positionL1==127){
//					positionL1=0;
//					pos1=0;
//			}
//    
//}

//int pos2 = 256;

//void DesplazaLetra2(void){

//      LCD_Clean();
//	
//      if(positionL2<384){   //MAXIMO DE LA PAGINA3 
//        positionL2=pos2;
//        LCD_symbolToLocalBuffer_L2('@');
//        }
//			
//				 LCD_update();
//				 pos2++;
//				
//				if (positionL2==384){
//					positionL2=256;
//					pos2=256;
//			}
//}

//int posComp = 0;
//void DesplazarLetraSeleccionada(uint8_t line){
//	
//	   LCD_Clean();
//	
//	  if (line==0){
//      if(positionL1<127){   //MAXIMO DE LA PAGINA1 
//        positionL1=posComp;
//        LCD_symbolToLocalBuffer_L1('@');
//        }
//			
//				 LCD_update();
//				 posComp++;
//				
//				if (positionL1==127){
//					positionL1=0;
//					posComp=0;
//			}
//		}
//		
//		if(line==1){   //MAXIMO DE LA PAGINA3 
//      if(positionL2<384){   //MAXIMO DE LA PAGINA3 
//        positionL2=posComp+256;
//        LCD_symbolToLocalBuffer_L2('@');
//        }
//			
//				 LCD_update();
//				 posComp++;
//				
//				if (positionL2==384){
//					positionL2=256;
//					posComp=0;
//			}
//    }

//}
//void prueba(void){
//  LCD_Clean();
//for (i=0; i<512; i++){
//   if (i & 0x01) buffer[i]=0x55;
//   else buffer[i]=0xAA;
//}
//LCD_update();
//}
//	
//int k=0;
//int pos=0;
//int posy=64;
//int posx=128;
//int posz=192;
//int swap=0;

//void Cuadrados(void){

//  if (swap==0){
//  for(k=pos;k<8+pos;k++){
//    buffer[k+pos]=0xFF;
//  }
//  
//  for(k=posx;k<8+posx;k++){
//    buffer[k+posx]=0xFF;
//  }
//    pos = pos + 8;
//    posx = posx + 8;
//  swap=1;
//    if (pos == 256){
//    pos=0;
//    posx=128;
//    posy=64;
//    posz=192;
//      LCD_Clean();
//  }
//}
//  else{
//  for(k=posy;k<8+posy;k++){
//    buffer[k+posy+8]=0xFF;
//  }
//  
//  for(k=posz;k<8+posz;k++){
//    buffer[k+posz+8]=0xFF;
//  }
//    posy = posy + 8;
//    posz = posz + 8;
//  swap=0;
//    if (posz == 264 ){
//    pos=0;
//    posx=128;
//    posy=64;
//    posz=192;
//      LCD_Clean();
//  }
//}

//  LCD_update();
//}

int pos_exam0=0;  	//Las dos primeras paginas van de 0-127(pagina 0) 127-255(pagina 1)
int pos_exam1=128+8;  
int pos_exam2=256;	//Las dos ultimas paginas van de 256-383(pagina 2) 384-511(pagina 3)
int pos_exam3=384+8;  
int swap_examen=0;
int b=0;

void Examen_BT2(void){ //Pinta el patron del examen
	
	if(swap_examen==0){
	
	if(pos_exam0 == 128){
	  pos_exam0 = 0;
	}
	if (pos_exam2 == 384){
		pos_exam2 = 256;
	}
	
	for(b = pos_exam0; b < pos_exam0 + 8; b++){
		buffer[b] = 0xFF;
	}
	
	for(b = pos_exam2; b < pos_exam2+8; b++){
		buffer[b] = 0xFF;
	}
	
	pos_exam0 = pos_exam0 + 16;
	pos_exam2 = pos_exam2 + 16;
	
	swap_examen=1;
	
} else {
		

	if(pos_exam1 == 256){
	  pos_exam1 = 128;
	}
	if (pos_exam3		== 512){
		pos_exam3 = 384;
	}
	
	for(b = pos_exam1; b < pos_exam1 + 8; b++){
		buffer[b] = 0xFF;
	}
	
	for(b = pos_exam3; b < pos_exam3+8; b++){
		buffer[b] = 0xFF;
	}
	
	pos_exam1 = pos_exam1 + 16;
	pos_exam3 = pos_exam3 + 16;

	swap_examen=0;
		
	}

	LCD_update();
}
int c = 0;
uint8_t sum=0x01;
int pag = 0;


int pos1_exam0=0;  	//Las dos primeras paginas van de 0-127(pagina 0) 127-255(pagina 1)
int pos1_exam1=128+8;  
int pos1_exam2=256;	//Las dos ultimas paginas van de 256-383(pagina 2) 384-511(pagina 3)
int pos1_exam3=384+8;  
int swap1_examen=0;
int b1=0;
int pag1=0;
int contador=0;
int j=0;

void Examen_BT2_Posible(void){ //Pinta el patron del examen
		
	if (pag1 == 0){
		
	  for(b = pos1_exam0; b < pos1_exam0 + 8; b++){
		  buffer[b] = 0xFF;
	  }
		
		pos1_exam0 = pos1_exam0 + 16;
		
		if (pos1_exam0 == 128){
	  	pag1++;
		  pos1_exam0 = 128+8;
		}
		
  }else if(pag1 == 1){

	  for(b = pos1_exam0; b < pos1_exam0 + 8; b++){
		  buffer[b] = 0xFF;
	  }
		
		pos1_exam0 = pos1_exam0 + 16;
		
		if (pos1_exam0 == 256+8){
	  	pag1++;
			pos1_exam0=256;
		}
		
	} else if (pag1 == 2){

	  for(b = pos1_exam0; b < pos1_exam0 + 8; b++){
		  buffer[b] = 0xFF;
	  }
		
		pos1_exam0 = pos1_exam0 + 16;
		
		if (pos1_exam0 == 384){
	  	pag1++;	
			pos1_exam0=384+8;	
		}
		
		
	}else {
		
		for(b = pos1_exam0; b < pos1_exam0 + 8; b++){
		  buffer[b] = 0xFF;
	  }
		
		pos1_exam0 = pos1_exam0 + 16;
		
		if (pos1_exam0 == 512+8){
	  	pag1=0;
			pos1_exam0=0;	
	}
}
		LCD_update();
	
}
void Examen_BT2_Posible_2(void){ //Pinta el patron del examen
		
	for(contador=0 ; contador < 8; contador++){
	
	if (pag1 == 0){
		
	  for(b = pos1_exam0; b < pos1_exam0 + 8; b++){
		  buffer[b] = 0xFF;

	  }
		
		pos1_exam0 = pos1_exam0 + 16;
		
		if(pos1_exam0 == 128){
			pos1_exam0 = pos1_exam0 + 8;
		}
		
  } else if (pag1 == 1){
		
		for(b = pos1_exam0; b < pos1_exam0 + 8; b++){
			
		  buffer[b] = 0xFF;
	  }
		pos1_exam0 = pos1_exam0 + 16;
		
		if(pos1_exam0 == 256+8){
			pos1_exam0 =256;
		}
		
		
	} else if (pag1 == 2){
		
		for(b = pos1_exam0; b < pos1_exam0 + 8; b++){
			
		  buffer[b] = 0xFF;
	  }
		pos1_exam0 = pos1_exam0 + 16;
		
		if(pos1_exam0 == 384){
			pos1_exam0 =384+8;
		}
		
	} else if (pag1 == 3){
		
		for(b = pos1_exam0; b < pos1_exam0 + 8; b++){
			
		  buffer[b] = 0xFF;
	  }
		
		pos1_exam0 = pos1_exam0 + 16;
		
		if(pos1_exam0 == 512+8){
			pos1_exam0 =0;
		}
	}
	
}
	
	pag1++;

	if (pag1 == 4){
	  pos1_exam0 = 0;
	  pag1 = 0;
	}
	LCD_update();
	
}


void barrido_arriba_abajo(void){
	
	LCD_Clean();
	
	if (pag == 0){
	for(c = 0; c < 128 ; c++){
		buffer[c] = sum;
	}
	sum = sum<<1; //numero que pones es las posiciones que desplazas
	
	if(sum == 0x00){
		pag++;
		sum = 0x01;
	}
	
} else if(pag == 1){

	for(c = 128; c < 256 ; c++){
		buffer[c] = sum;
	}
	sum = sum<<1; //numero que pones es las posiciones que desplazas
	
	if(sum == 0x00){
		pag++;
		sum = 0x01;
	}
	
}else if(pag == 2){
	
		for(c = 256; c < 384 ; c++){
		buffer[c] = sum;
	}
		
	sum = sum<<1; //numero que pones es las posiciones que desplazas
	
	if(sum == 0x00){
		pag++;
		sum = 0x01;
	}
}else{

	for(c = 384; c < 512 ; c++){
		buffer[c] = sum;
	}
	sum = sum<<1; //numero que pones es las posiciones que desplazas
	
	if(sum == 0x00){
		pag=0;
		sum = 0x01;
	}
}

	
	LCD_update();
}
void barrido_izq_der(void){
			LCD_Clean();
	
		buffer[j]    = 0xFF;
		buffer[j+128]= 0xFF;
		buffer[j+256]= 0xFF;
		buffer[j+384]= 0xFF;
		
	j++;
	
	if (j ==128){j=0;}
	
		LCD_update();
	
}
int p=0;
void pintar_pelota(void){
	for(p = 0 ; p < 8 ; p++){
		
		buffer[p] = 0xFF;
	}
	
	LCD_update();
}
	
int posision=0;
int paginasion=0;

void arriba(void){
	
	if(posision > 128){
		LCD_Clean();
		paginasion--;
		posision = posision - 128;
		
		for(p = posision ; p < posision + 8 ; p++){
		  buffer[p] = 0xFF;
	  }
		

		LCD_update();
		
	} 
}

void abajo(void){
	
	if(posision < 384){
		LCD_Clean();
		paginasion++;
		posision = posision + 128;
		
		for(p = posision ; p < posision + 8 ; p++){
		  buffer[p] = 0xFF;
	  }
		

		LCD_update();
		
	} 
}

void derecha(void){
	
	if(paginasion == 0 && posision < 128-8){
		LCD_Clean();

		posision = posision + 8;
		
		for(p = posision ; p < posision + 8 ; p++){
		  buffer[p] = 0xFF;
			
	  }
	}
		
	if(paginasion == 1 && posision < 256-8){
		LCD_Clean();

		posision = posision + 8;
		
		for(p = posision ; p < posision + 8 ; p++){
		  buffer[p] = 0xFF;
			
	  }
	}		
			if(paginasion == 2 && posision < 384-8){
		LCD_Clean();

		posision = posision + 8;
		
		for(p = posision ; p < posision + 8 ; p++){
		  buffer[p] = 0xFF;
			
	  }
	}
		
	  if(paginasion == 3 && posision < 512-8){
		LCD_Clean();

		posision = posision + 8;
		
		for(p = posision ; p < posision + 8 ; p++){
		  buffer[p] = 0xFF;
			
	  }
	}
		
		LCD_update();
	
  }
	


void izquierda(void){
	
	if(paginasion == 0 && posision >= 0+8){
		LCD_Clean();

		posision = posision - 8;
		
		for(p = posision ; p < posision + 8 ; p++){
		  buffer[p] = 0xFF;
			
	  }
	}
		
	if(paginasion == 1 && posision >= 128+8){
		LCD_Clean();

		posision = posision - 8;
		
		for(p = posision ; p < posision + 8 ; p++){
		  buffer[p] = 0xFF;
			
	  }
	}		
			if(paginasion == 2 && posision >= 256+8){
		LCD_Clean();

		posision = posision - 8;
		
		for(p = posision ; p < posision + 8 ; p++){
		  buffer[p] = 0xFF;
			
	  }
	}
		
	  if(paginasion == 3 && posision >= 384+8){
		LCD_Clean();

		posision = posision - 8;
		
		for(p = posision ; p < posision + 8 ; p++){
		  buffer[p] = 0xFF;
			
	  }
	}
		
		LCD_update();
	
  }