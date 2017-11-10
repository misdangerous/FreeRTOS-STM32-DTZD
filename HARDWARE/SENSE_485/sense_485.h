#ifndef __SENSE_485_H
#define __SENSE_485_H

#include "sys.h"

#define BUFFLEN 100
#define Complete 0x00
#define Busy 0xff


typedef struct 
{
	uint8_t DataLen;
	uint8_t Buff[BUFFLEN];
}Modbus_TypeDef;


struct Sense
{
	Modbus_TypeDef Rece;
	Modbus_TypeDef Send;
};

void Sense_485_Init(u32 bound);
void DMA_InitCfg(void);
void USART3_InitCfg(u32 bound);
void UartSend(uint8_t *buf,uint8_t len);
uint8_t UartRece(uint8_t *pbuffer,uint8_t *len);
void SenseRelase(void);
#endif
