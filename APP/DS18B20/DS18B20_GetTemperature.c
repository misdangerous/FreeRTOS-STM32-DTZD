
#include "DS18B20_GetTemperature.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "eeprom_save.h"
//MAXNUM在ds18b20.h中定义
struct DS18B20Temp DS18B20Temp;

static void Init_DS18B20(struct DS18B20Temp * pDS18B20);

void vDS18B20GetTemperatureTask( void *pvParameters )
{
	uint8_t i;
	Init_DS18B20(&DS18B20Temp);
	while(1)
	{
		taskENTER_CRITICAL();           //进入临界区
		for(i=0;i<DS18B20Temp.SetNum;i++)
		{
			if(DS18B20Temp.TempSense[i].status == DS18B20_DISCONNECT)
			{
				DS18B20Temp.TempSense[i].Data = 0;
			}
			else
			{
				DS18B20Temp.TempSense[i].Data = DS18B20_ReadDesignateTemper(DS18B20Temp.TempSense[i].ID);
			}
		}
		taskEXIT_CRITICAL();            //退出临界区 
		vTaskDelay(1000);
	}
}

static void Init_DS18B20(struct DS18B20Temp * pDS18B20)
{
	uint8_t i,j,k,addrlen,temp;
	uint8_t pbuffer[200];
	uint8_t ID_Buff[MAXNUM][8]; 
	
	taskENTER_CRITICAL();           //进入临界区
	DS18B20_Init();
	pDS18B20->OnlineNum = DS18B20_SearchROM(ID_Buff,MAXNUM);
	Computer_485_ModeTx();
	printf("Search DS18B20 Number is %d\r\n",pDS18B20->OnlineNum); 
	Computer_485_ModeRx();
	taskEXIT_CRITICAL();            //退出临界区
	
	EEPROM_Get_Addr(EEPROM_DS18B20_Addr,pbuffer,&addrlen);
	pDS18B20->SetNum = pbuffer[0];
	for(i=0;i<pbuffer[0];i++)
	{
		pDS18B20->TempSense[i].status = DS18B20_CONNECT;
		for(j=0;j<8;j++)
		{
			pDS18B20->TempSense[i].ID[j] = pbuffer[i*8+j+1];
		}
	}
	
	for(i=0;i<addrlen;i++)//查找设置的每一个探头在线状态
	{
		for(k=0;k<pDS18B20->OnlineNum;k++)
		{
			for(j=0;j<8;j++)
			{
				if(pDS18B20->TempSense[i].ID[j] != ID_Buff[k][j])
				{
					temp = 1;
					break;
				}
				else
				{
					temp = 0;
				}
			}
			if(!temp)
			{
				break;
			}
		}
		if(temp)
		{
			pDS18B20->TempSense[i].status = DS18B20_DISCONNECT;
		}
	}
}

void Search_DS18B20(uint8_t *searchnumber,u8 (*pID)[8])
{
	taskENTER_CRITICAL();           //进入临界区
	*searchnumber = DS18B20_SearchROM(pID,MAXNUM);
	taskEXIT_CRITICAL();            //退出临界区
}

