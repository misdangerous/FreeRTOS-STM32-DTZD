#ifndef __DS18B20_GETTEMPERATURE_H
#define __DS18B20_GETTEMPERATURE_H

#include "ds18b20.h"

typedef enum
{
	DS18B20_DISCONNECT,
	DS18B20_CONNECT
}DS18B20Status_TypeDef;

typedef struct 
{
	uint8_t ID[8];
	short Data;
	DS18B20Status_TypeDef status;
}DS18B20Temp_TypeDef;

struct DS18B20Temp
{
	uint8_t OnlineNum;
	uint8_t SetNum;
	DS18B20Temp_TypeDef TempSense[MAXNUM];
};
extern struct DS18B20Temp  DS18B20Temp;

void vDS18B20GetTemperatureTask( void *pvParameters );
void Search_DS18B20(uint8_t *searchnumber,u8 (*pID)[8]);
#endif
