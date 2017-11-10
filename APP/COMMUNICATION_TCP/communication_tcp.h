#ifndef __COMMUNICATION_TCP_H
#define __COMMUNICATION_TCP_H

#include "sys.h"


#define Machine_Addr_Default 2
#define Machine_REV  0

#define DEFAULT_REPORTTIME_DS18B20      2000
#define DEFAULT_REPORTTIME_INFRAREDTEMP 2000
#define DEFAULT_REPORTTIME_CURRENT      2000
#define DEFAULT_REPORTTIME_VIBRATOR     2000
#define DEFAULT_REPORTTIME_SWITCH       2000
#define DEFAULT_REPORTTIME_HEARTBEAT    2000
typedef enum
{
	Machine_Register=0,
	DS18B20_Register,
	DS18B20_Data,
	InfraredTemp_Register,
	InfraredTemp_Data,
	Current_Register,
	Current_Data,
	Vibrator_Register,
	Vibrator_Data,
	Switch_Data,
	Error_Code,
	Cmd_ChangeTime=0x80,
	Cmd_SwitchOutput,
	Cmd_MachineUserTime,
	HeartBeat_Frame=0xF0,
}ReportCode_TypeDef;

void vTCP_SendCommTask( void *pvParameters );
void vTCP_ReceCommTask( void *pvParameters );
uint8_t ReportCmd(ReportCode_TypeDef function,uint8_t *pBuffer,uint8_t len,uint8_t errror);
void vTCP_ReconnectSockeTask( void *pvParameters );

#endif

