#ifndef __USART_2_4_5_H
#define __USART_2_4_5_H 
#include "sys.h"  



extern u8 USART2_Car1_jsonParseBuF[300]; 
extern u8 USART4_Getter_jsonParseBuF[300]; 
extern u8 USART5_Car2_jsonParseBuF[300];


extern unsigned char const crc8_tab[256];
unsigned	char crc8_calculate(unsigned char * ucPtr, unsigned char ucLen) ;


void uart2_init(u32 bound); //串口2初始化 
void usart2_send(u8 data);
void usart2_sendString(char *data,u8 len);

//串口4
//TX:PC10   RX:PC11
void uart4_init(u32 bound); //串口4初始化 
void uart4_send(u8 data);
void uart4_sendString(char *data,u8 len);

//串口5
//RX: PD2  TX:  PC10
void uart5_init(u32 bound); //串口4初始化 
void uart5_send(u8 data);
void uart5_sendString(char *data,u8 len);

#endif













