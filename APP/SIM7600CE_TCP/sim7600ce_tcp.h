#ifndef __SIM7600CE_TCP_H
#define __SIM7600CE_TCP_H

#include "sys.h"


typedef enum
{
	SIM7600CE_CONNECT=0,
	SIM7600CE_CLOSE
}Online_TypeDef;

typedef enum
{
	ECHO_OFF=0,
	CICCID,
	CGSOCKCONT,
	CSOCKSETPN,
	CIPMODE,
	NETOPEN,
	CIPOPEN,
}SIM7600CE_SetFlag;

typedef enum
{
	NoError=0,
	SetError_ECHO_OFF,
	Error_NoSIM,
	SetError_CGSOCKCONT,
	SetError_CSOCKSETPN,
	SetError_CIPMODE,
	SetError_NETOPEN,
	SetError_CONNECT,
}ErrorTypeDef;

struct SIM7600CE_Info
{
	ErrorTypeDef Error;
	Online_TypeDef Online;
	/*设置SOCKET标志位，内部值在头文件枚举结构体中定义*/
	SIM7600CE_SetFlag SIM7600CE_Flag;
	uint8_t ICCID_Data[20];
};
extern struct SIM7600CE_Info SIM7600CE_Info;

void vSIM7600CE_TCPTask( void *pvParameters );
void vTCP_Rece_Task( void *pvParameters );
void WaitSIM7600CE_Init(char *pStrCharSuccess);
#endif
