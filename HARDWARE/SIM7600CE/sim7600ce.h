#ifndef __SIM7600CE_H
#define __SIM7600CE_H


#include "sys.h"
#include "FreeRTOS.h"

#define SIM7600CE_BUFFLEN 512
#define BUFFER_DEPTH      10

typedef struct 
{
	uint8_t DataLen;
	uint8_t Buff[SIM7600CE_BUFFLEN];
}Modbus_TypeDef;


struct SIM7600CE
{
	Modbus_TypeDef Rece;
	Modbus_TypeDef Send;
};
extern struct SIM7600CE_Info SIM7600CE_Info;

extern struct SIM7600CE SIM7600CE_Modbus;

void SIM7600CE_PWRKEY(void);
void SIM7600CE_RESET(void);

void SIM7600CE_Init(u32 bound);
void USART2_DMA_InitCfg(void);
void USART2_InitCfg(u32 bound);
void SIM7600CESend(uint8_t *buf,uint8_t len);
uint8_t SIM7600CERece(uint8_t *pbuffer,uint8_t *len,TickType_t pportMAX_DELAY);
uint8_t CMD_Respose(char *pCMD,char *pRespose,TickType_t pportMAX_DELAY);
uint8_t RECONNECT_CMD_Respose(char *pCMD,char *pRespose,TickType_t pportMAX_DELAY);

#endif
