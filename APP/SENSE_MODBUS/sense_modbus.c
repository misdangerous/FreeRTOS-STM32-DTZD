
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#include "sense_modbus.h"
#include "sense_485.h"
#include "Check_CRC.h"
#include "eeprom_save.h"
#include <string.h>

extern EventGroupHandle_t SwitchEventGroupHandler;	//事件标志组句柄

/*位的定位必须是连续的，注意其他文件中的定位也要同时改变*/
#define SWITCH_INPUT1_BIT	         (1<<0)				//事件位
#define SWITCH_INPUT2_BIT            (1<<1)
#define SWITCH_INPUT3_BIT            (1<<2)
#define SWITCH_INPUT4_BIT            (1<<3)
#define SWITCH_INPUT5_BIT            (1<<4)
#define SWITCH_INPUT6_BIT            (1<<5)
#define SWITCH_INPUT7_BIT            (1<<6)
//#define SWITCH_INPUT8_BIT            (1<<7)
//#define SWITCH_INPUT9_BIT            (1<<8)
//#define SWITCH_INPUT10_BIT           (1<<9)


uint8_t Sense_Modbus_Buffer[BUFFLEN];
struct Sense_Data Sense_Data;

static void Sense_InitAddr(void)
{
	uint8_t i,eepromlen;
	uint8_t pBuffer[CURRENT_NUM*2+VIBRATOR_NUM*2+INFRAREDTEMP_NUM*2];
	if(EEPROM_Get_Addr(EEPROM_Current_Addr,pBuffer,&eepromlen))
	{
		for(i=0;i<CURRENT_NUM;i++)
		{
			Sense_Data.Current[i].Addr = pBuffer[0+2*i]<<8 | pBuffer[1+2*i];
		}
	}
	if(EEPROM_Get_Addr(EEPROM_Vibrator_Addr,pBuffer,&eepromlen))
	{
		for(i=0;i<VIBRATOR_NUM;i++)
		{
			Sense_Data.Vibrator[i].Addr = pBuffer[0+2*i]<<8 | pBuffer[1+2*i];
		}
	}
	if(EEPROM_Get_Addr(EEPROM_InfraredTemp_Addr,pBuffer,&eepromlen))
	{
		for(i=0;i<INFRAREDTEMP_NUM;i++)
		{
			Sense_Data.InfraredTemp[i].Addr = pBuffer[0+2*i]<<8 | pBuffer[1+2*i];
		}
	}
}

void vSense_ModbusTask( void *pvParameters )
{
	Sense_485_Init(4800);
	Sense_InitAddr();
	while(1)
	{
		ReadData(&Sense_Data);
	}
}

static void SenseRequest(uint16_t addr,uint8_t function)
{
	uint16_t CrcData;
	Sense_Modbus_Buffer[0] = addr >> 8;
	Sense_Modbus_Buffer[1] = addr;
	Sense_Modbus_Buffer[2] = function;
	Sense_Modbus_Buffer[3] = 0x07;
	Sense_Modbus_Buffer[4] = SENSE_MODBUS_REC;
	CrcData = MBCrcCheck(Sense_Modbus_Buffer,5);
	Sense_Modbus_Buffer[5] = CrcData >> 8;
	Sense_Modbus_Buffer[6] = CrcData;
	UartSend(Sense_Modbus_Buffer,7);
}
static uint8_t SenseAsk(uint8_t *pBuffer,uint8_t len,uint16_t addr,uint8_t function,uint8_t code)
{
	uint16_t Addrtemp;
//	if(ReceCrcCheck(pBuffer,len))
//	{
		Addrtemp = (pBuffer[0] << 8) | pBuffer[1];
		if((Addrtemp == addr)&&(pBuffer[2] == 0xFF))
		{
			if((pBuffer[4] == function)&&(pBuffer[5] == 0x00))
			{
				switch(pBuffer[0])
				{
					case CURRENT_FUNCTION:
											Sense_Data.Current[code].Data = (pBuffer[6] << 8) | pBuffer[7];
											Sense_Data.Current[code].Data = Sense_Data.Current[code].Data * 10;
											break;
					case INFRAREDTEMP_FUNCTION:
											Sense_Data.InfraredTemp[code].Data = (pBuffer[6] << 8) | pBuffer[7];
											Sense_Data.InfraredTemp[code].Data = Sense_Data.InfraredTemp[code].Data * 100;
											break;
					case VIBRATOR_FUNCTION:
											Sense_Data.Vibrator[code].AverData[x] = (pBuffer[7] << 8)  | pBuffer[8];
											Sense_Data.Vibrator[code].AverData[y] = (pBuffer[9] << 8)  | pBuffer[10];
											Sense_Data.Vibrator[code].AverData[z] = (pBuffer[11] << 8) | pBuffer[12];
											Sense_Data.Vibrator[code].MaxData[x] =  (pBuffer[14] << 8) | pBuffer[15];
											Sense_Data.Vibrator[code].MaxData[y] =  (pBuffer[16] << 8) | pBuffer[17];
											Sense_Data.Vibrator[code].MaxData[z] =  (pBuffer[18] << 8) | pBuffer[19];
											break;
				}
				return 1;
			}
		}
//	}
	return 0;
}
void ReadData(struct Sense_Data * pData)
{
	uint8_t i,j,len;
	BaseType_t err=pdFALSE;
	char DisplayStr[100];
	EventBits_t SwitchEventValue;
	uint8_t ActionNum;
	
	for(i=0;i<VIBRATOR_NUM;i++)
	{
		SenseRequest(pData->Vibrator[i].Addr,VIBRATOR_FUNCTION);
		err = UartRece(Sense_Modbus_Buffer,&len);
		if(err == pdPASS)
		{
			SenseAsk(Sense_Modbus_Buffer,len,pData->Vibrator[i].Addr,VIBRATOR_FUNCTION,i);
		}
		else
		{
			for(j=0;j<3;j++)
			{
				pData->Vibrator[i].AverData[j] = 0;
			}
			for(j=0;j<3;j++)
			{
				pData->Vibrator[i].MaxData[j] = 0;
			}
		}
		vTaskDelay(100);
	}
	for(i=0;i<CURRENT_NUM;i++)
	{
		SenseRequest(pData->Current[i].Addr,INFRAREDTEMP_FUNCTION);
		err = UartRece(Sense_Modbus_Buffer,&len);
		if(err == pdPASS)
		{
			SenseAsk(Sense_Modbus_Buffer,len,pData->Current[i].Addr,INFRAREDTEMP_FUNCTION,i);
		}
		else
		{
			pData->Current[i].Data = 0;
		}
		vTaskDelay(100);
	}
	for(i=0;i<INFRAREDTEMP_NUM;i++)
	{
		SenseRequest(pData->InfraredTemp[i].Addr,INFRAREDTEMP_FUNCTION);
		err = UartRece(Sense_Modbus_Buffer,&len);
		if(err == pdPASS)
		{
			SenseAsk(Sense_Modbus_Buffer,len,pData->InfraredTemp[i].Addr,INFRAREDTEMP_FUNCTION,i);
		}
		else
		{
			pData->InfraredTemp[i].Data = 0;
		}
		vTaskDelay(100);
	}
	if(SwitchEventGroupHandler!=NULL)
	{
		ActionNum = 0;
		SwitchEventValue = xEventGroupGetBits(SwitchEventGroupHandler);
		for(i=0;i<SWITCH_INPUT_NUM;i++)
		{
			if((SwitchEventValue >> i) & 0x01)
			{
				ActionNum++;
			}
		}
		sprintf(DisplayStr,"display error code is \"%02X\"\r\n",ActionNum);
		UartSend((uint8_t *)DisplayStr,(uint8_t)strlen(DisplayStr));
		vTaskDelay(500);
	}
}


