#ifndef __SENSE_MODBUS_H
#define __SENSE_MODBUS_H

#include "sys.h"
 
#define SENSE_MODBUS_REC  0

#define CURRENT_NUM       4
#define INFRAREDTEMP_NUM  2
#define VIBRATOR_NUM      2

#define CURRENT_FUNCTION      0x01
#define INFRAREDTEMP_FUNCTION 0x02
#define VIBRATOR_FUNCTION     0x03

#define x 0
#define y 1
#define z 2

typedef struct 
{
	uint16_t Addr;
	uint16_t Data;
}Sense_TypeDef;

typedef struct 
{
	uint16_t Addr;
	uint16_t MaxData[3];
	uint16_t AverData[3];
}SenseVibrator_TypeDef;

struct Sense_Data
{
	Sense_TypeDef Current[CURRENT_NUM];
	Sense_TypeDef InfraredTemp[INFRAREDTEMP_NUM];
	SenseVibrator_TypeDef Vibrator[VIBRATOR_NUM];
};

extern struct Sense_Data Sense_Data;

void vSense_ModbusTask( void *pvParameters );
void ReadData(struct Sense_Data * pData);
#endif
