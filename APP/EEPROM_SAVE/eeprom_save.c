#include "FreeRTOS.h"
#include "task.h"
#include "eeprom_save.h"
#include "at24cxx.h"
#include "semphr.h"
#include <string.h>
#include "Check_CRC.h"

/**���ȵȼ��ϵͣ��ȴ�������������֮���ٽ��д洢**/

extern SemaphoreHandle_t EEPROMBinarySemaphore;	//��ֵ�ź������

uint8_t EEPROMBuffer[200];

static void EEPROM_GetData(uint16_t paddr,uint8_t *pBuffer,uint8_t len)
{
	BaseType_t err=pdFALSE;
	err=xSemaphoreTake(EEPROMBinarySemaphore,portMAX_DELAY);	//��ȡ�ź���
	if(err == pdTRUE)
	{
		taskENTER_CRITICAL();           //�����ٽ���
		AT24Cxx_ReadSerialByte(paddr,pBuffer,len);
		taskEXIT_CRITICAL();            //�˳��ٽ���
	}
	xSemaphoreGive(EEPROMBinarySemaphore);	//�ͷŶ�ֵ�ź���
}
/***********************************************************
��ȡEEPROM�еĵ�ַ�����ֽ���ǰ
**********************************************************/
uint8_t EEPROM_Get_Addr(EEPROM_Save_TypeDef function,uint8_t *pBuffer,uint8_t *Getlen)
{
	uint16_t len,i;
	memset(EEPROMBuffer,0,sizeof(EEPROMBuffer));
	switch(function)
	{
		case EEPROM_Machine_Addr:
			EEPROM_GetData(Machine_Save_Addr,EEPROMBuffer,Machine_SaveAddr_Len);
			len = Machine_SaveAddr_Len;
			break;
		case EEPROM_Current_Addr:
			EEPROM_GetData(Current_Save_Addr,EEPROMBuffer,Current_SaveAddr_Len);
			len = Current_SaveAddr_Len;
			break;
		case EEPROM_InfraredTemp_Addr:
			EEPROM_GetData(InfraredTemp_Save_Addr,EEPROMBuffer,InfraredTemp_SaveAddr_Len);
			len = InfraredTemp_SaveAddr_Len;
			break;
		case EEPROM_Vibrator_Addr:
			EEPROM_GetData(Vibrator_Save_Addr,EEPROMBuffer,Vibrator_SaveAddr_Len);
			len = Vibrator_SaveAddr_Len;
			break;
		case EEPROM_DS18B20_Addr:
			EEPROM_GetData(DS18B20_Save_Addr,EEPROMBuffer,DS18B20_SaveAddr_Len);
			len = EEPROMBuffer[0]*8+3;//ÿ��8λ�������ټ�2λCRC��1λ���߳���
			break;
		case EEPROM_DS18B20_SendTime:
			EEPROM_GetData(DS18B20_Save_SendTime,EEPROMBuffer,DS18B20_SaveSendTime_Len);
			len = DS18B20_SaveSendTime_Len;
			break;
		case EEPROM_Current_SendTime:
			EEPROM_GetData(Current_Save_SendTime,EEPROMBuffer,Current_SaveSendTime_Len);
			len = Current_SaveSendTime_Len;
			break;
		case EEPROM_InfraredTemp_SendTime:
			EEPROM_GetData(InfraredTemp_Save_SendTime,EEPROMBuffer,InfraredTemp_SaveSendTime_Len);
			len = InfraredTemp_SaveSendTime_Len;
			break;
		case EEPROM_Vibrator_SendTime:
			EEPROM_GetData(Vibrator_Save_SendTime,EEPROMBuffer,Vibrator_SaveSendTime_Len);
			len = Vibrator_SaveSendTime_Len;
			break;
		case EEPROM_Switch_SendTime:
			EEPROM_GetData(Switch_Save_SendTime,EEPROMBuffer,Switch_SaveSendTime_Len);
			len = Switch_SaveSendTime_Len;
			break;
		case EEPROM_HeartBeat_SendTime:
			EEPROM_GetData(HeartBeat_Save_SendTime,EEPROMBuffer,HeartBeat_SaveSendTime_Len);
			len = HeartBeat_SaveSendTime_Len;
			break;
	}
	if(ReceCrcCheck(EEPROMBuffer,len))
	{
		for(i=0;i<(len-2);i++)
		{
			pBuffer[i] = EEPROMBuffer[i];
		}
		*Getlen = len-2;
		return 1;
	}
	return 0;
}

static uint8_t EEPROM_ChangeData(uint16_t paddr,uint8_t *pBuffer,uint8_t len)
{
	BaseType_t err=pdFALSE;
	uint8_t pbuff[200],i;
	err=xSemaphoreTake(EEPROMBinarySemaphore,portMAX_DELAY);	//��ȡ�ź���
	if(err == pdTRUE)
	{
		taskENTER_CRITICAL();           //�����ٽ���
		AT24Cxx_WriteSerialByte(paddr,pBuffer,len);
		AT24Cxx_ReadSerialByte(paddr,pbuff,len);
		taskEXIT_CRITICAL();            //�˳��ٽ���
	}
	xSemaphoreGive(EEPROMBinarySemaphore);	//�ͷŶ�ֵ�ź���
	for(i=0;i<len;i++)
	{
		if(pbuff[i] != pBuffer[i])
			return 0;
	}
	return 1;
}

uint8_t EEPROM_Change_Addr(EEPROM_Save_TypeDef function,uint8_t *pBuffer,uint8_t len)
{
	uint16_t i;
	uint16_t CrcData;
	memset(EEPROMBuffer,0,sizeof(EEPROMBuffer));
	for(i=0;i<len;i++)
	{
		EEPROMBuffer[i] = pBuffer[i];
	}
	CrcData = MBCrcCheck(EEPROMBuffer,len);
	EEPROMBuffer[len] = CrcData >> 8;
	EEPROMBuffer[len+1] = CrcData;
	switch(function)
	{
		case EEPROM_Machine_Addr:
			return EEPROM_ChangeData(Machine_Save_Addr,EEPROMBuffer,Machine_SaveAddr_Len);
		case EEPROM_Current_Addr:
			return EEPROM_ChangeData(Current_Save_Addr,EEPROMBuffer,Current_SaveAddr_Len);
		case EEPROM_InfraredTemp_Addr:
			return EEPROM_ChangeData(InfraredTemp_Save_Addr,EEPROMBuffer,InfraredTemp_SaveAddr_Len);
		case EEPROM_Vibrator_Addr:
			return EEPROM_ChangeData(Vibrator_Save_Addr,EEPROMBuffer,Vibrator_SaveAddr_Len);
		case EEPROM_DS18B20_Addr:
			return EEPROM_ChangeData(DS18B20_Save_Addr,EEPROMBuffer,len+2); //�洢0��ַΪ̽ͷ�������Ժ�ÿ�˸�Ϊһ��̽ͷ�ĵ�ַ
		case EEPROM_DS18B20_SendTime:
			return EEPROM_ChangeData(DS18B20_Save_SendTime,EEPROMBuffer,DS18B20_SaveSendTime_Len);
		case EEPROM_Current_SendTime:
			return EEPROM_ChangeData(Current_Save_SendTime,EEPROMBuffer,Current_SaveSendTime_Len);
		case EEPROM_InfraredTemp_SendTime:
			return EEPROM_ChangeData(InfraredTemp_Save_SendTime,EEPROMBuffer,InfraredTemp_SaveSendTime_Len);
		case EEPROM_Vibrator_SendTime:
			return EEPROM_ChangeData(Vibrator_Save_SendTime,EEPROMBuffer,Vibrator_SaveSendTime_Len);
		case EEPROM_Switch_SendTime:
			return EEPROM_ChangeData(Switch_Save_SendTime,EEPROMBuffer,Switch_SaveSendTime_Len);
		case EEPROM_HeartBeat_SendTime:
			return EEPROM_ChangeData(HeartBeat_Save_SendTime,EEPROMBuffer,HeartBeat_SaveSendTime_Len);
	}
	return 0;
}

void InitMachineAddr(uint16_t *pMachineAddr)
{
	uint8_t pbuffer[2],eepromlen;
	if(EEPROM_Get_Addr(EEPROM_Machine_Addr,pbuffer,&eepromlen))
	{
		*pMachineAddr = pbuffer[0]<<8 | pbuffer[1];
	}
}


