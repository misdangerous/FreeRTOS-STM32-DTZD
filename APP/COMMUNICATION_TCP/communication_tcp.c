
#include "communication_tcp.h"
#include "sim7600ce_tcp.h"
#include "sim7600ce.h"
#include "Check_CRC.h"
#include "sense_modbus.h"
#include "DS18B20_GetTemperature.h"
#include "eeprom_save.h"

#include <string.h>
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

extern QueueHandle_t SendMessage_Queue;	//信息队列句柄
extern QueueHandle_t ReceMessage_Queue;	//信息队列句柄

/*缓冲区的0地址为长度*/
uint8_t Send_Buffer[BUFFER_DEPTH][SIM7600CE_BUFFLEN];
uint8_t SendQueue_Count=0;
extern char *CIPOPEN_CMD;
extern char *CLOSE_CMD;
extern char *CONNECT_CMD;

extern EventGroupHandle_t TCPEventGroupHandler;	//事件标志组句柄
#define TCPSTARTBIT	               (1<<0)				//事件位
#define MACHINE_REGISTERBIT        (1<<1)
#define DS18B20_REGISTERBIT        (1<<2)
#define DS18B20_DATABIT            (1<<3)
#define INFRAREDTEMP_REGISTERBIT   (1<<4)
#define INFRAREDTEMP_DATABIT       (1<<5)
#define CURRENT_REGISTERBTI        (1<<6)
#define CURRENT_DATABIT            (1<<7)
#define VIBRATOR_REGISTERBIT       (1<<8)
#define VIBRATOR_DATABIT           (1<<9)
#define SWITCH_DATABIT             (1<<10)
#define ERROR_CODEBIT              (1<<11)
#define HEARTBEATBIT               (1<<12)
#define TCP_CLOSEBIT			   (1<<13)				//事件位

#define REGISTERALL                (MACHINE_REGISTERBIT | DS18B20_REGISTERBIT | INFRAREDTEMP_REGISTERBIT | CURRENT_REGISTERBTI | VIBRATOR_REGISTERBIT)

extern EventGroupHandle_t SwitchEventGroupHandler;	//事件标志组句柄
#define SWITCH_OUTPUT1_BIT           (1<<7)
#define SWITCH_OUTPUT2_BIT           (1<<8)
#define SWITCH_OUTPUT3_BIT           (1<<9)
#define SWITCH_OUTPUT4_BIT           (1<<10)
#define SWITCH_OUTPUT5_BIT           (1<<11)

extern TimerHandle_t 	DS18B20Timer_Handle;			
extern TimerHandle_t	InfraredTempTimer_Handle;		
extern TimerHandle_t	CurrentTimer_Handle;		    
extern TimerHandle_t	VibratorTimer_Handle;		    
extern TimerHandle_t	SwitchTimer_Handle;		       
extern TimerHandle_t	HeartBeatTimer_Handle;	


EventBits_t Machine_Register_CMD(TickType_t pportMAX_DELAY);
uint8_t ReceCmd(uint8_t *pBuffer,uint8_t len);
void ReceSperate(uint8_t *pBuffer,uint8_t len,uint8_t *pSperateNum);
static void Sense_Register(ReportCode_TypeDef function,uint8_t pOnlineNum);
static void SenseDS18B20_Register(void);
static void ChangeTimeExecute(uint8_t *pDatabuffer,uint8_t *pTimeBuffer);
static void SwitchOutputExecute(uint8_t *pDatabuffer,uint8_t *pTimeBuffer);

void vTCP_SendCommTask( void *pvParameters )
{
	if(TCPEventGroupHandler!=NULL)
	{
		xEventGroupWaitBits((EventGroupHandle_t	)TCPEventGroupHandler,		
							(EventBits_t		)TCPSTARTBIT,
							(BaseType_t			)pdFALSE,				
							(BaseType_t			)pdTRUE,
							(TickType_t			)portMAX_DELAY);
	}
	Machine_Register_CMD(portMAX_DELAY);
	xTimerStart(DS18B20Timer_Handle,0);
	xTimerStart(InfraredTempTimer_Handle,0);
	xTimerStart(CurrentTimer_Handle,0);
	xTimerStart(VibratorTimer_Handle,0);
	xTimerStart(SwitchTimer_Handle,0);
	xTimerStart(HeartBeatTimer_Handle,0);
	while(1)
	{
		xEventGroupWaitBits((EventGroupHandle_t	)TCPEventGroupHandler,		
							(EventBits_t		)HEARTBEATBIT,
							(BaseType_t			)pdTRUE,				
							(BaseType_t			)pdTRUE,
							(TickType_t			)portMAX_DELAY);
//		vTaskDelay(1000);
	}
}
void vTCP_ReceCommTask( void *pvParameters )
{
	BaseType_t err=pdFALSE;
	uint32_t pReceBuffer;
	uint8_t *precebuff;
	uint8_t len,SperateNum;
	if(TCPEventGroupHandler!=NULL)
	{
		xEventGroupWaitBits((EventGroupHandle_t	)TCPEventGroupHandler,		
							(EventBits_t		)TCPSTARTBIT,
							(BaseType_t			)pdFALSE,				
							(BaseType_t			)pdTRUE,
							(TickType_t			)portMAX_DELAY);
	}
	while(1)
	{
		err=xQueueReceive(ReceMessage_Queue,&pReceBuffer,portMAX_DELAY);//请求消息Message_Queue
		if(err==pdTRUE)			//接收到消息
		{
			precebuff = (uint8_t *)pReceBuffer;
			len = precebuff[0];
			if(strstr((char *)precebuff,CLOSE_CMD) != NULL)
			{
				xEventGroupSetBits(TCPEventGroupHandler,TCP_CLOSEBIT);//网络出现断线情况，停止数据上报，开始进行重新连接任务
				xTimerStop(DS18B20Timer_Handle,0);
				xTimerStop(InfraredTempTimer_Handle,0);
				xTimerStop(CurrentTimer_Handle,0);
				xTimerStop(VibratorTimer_Handle,0);
				xTimerStop(SwitchTimer_Handle,0);
				xTimerStop(HeartBeatTimer_Handle,0);
			}
			else
			{
	//			ReceCmd(&precebuff[1],len);
				ReceSperate(&precebuff[1],len,&SperateNum);
			}
		}
	}
}
void vTCP_ReconnectSockeTask( void *pvParameters )
{
	EventBits_t uxBits;
	if(TCPEventGroupHandler!=NULL)
	{
		xEventGroupWaitBits((EventGroupHandle_t	)TCPEventGroupHandler,		
							(EventBits_t		)TCPSTARTBIT,
							(BaseType_t			)pdFALSE,				
							(BaseType_t			)pdTRUE,
							(TickType_t			)portMAX_DELAY);
	}
	while(1)
	{
		xEventGroupWaitBits((EventGroupHandle_t	)TCPEventGroupHandler,		
							(EventBits_t		)TCP_CLOSEBIT,
							(BaseType_t			)pdFALSE,				
							(BaseType_t			)pdTRUE,
							(TickType_t			)portMAX_DELAY);
		if(CMD_Respose(CIPOPEN_CMD,CONNECT_CMD,portMAX_DELAY))
		{
			xEventGroupClearBits(TCPEventGroupHandler,TCP_CLOSEBIT);
			while((uxBits & REGISTERALL) != REGISTERALL)
			{
				uxBits = Machine_Register_CMD(10000);
				if(uxBits & TCP_CLOSEBIT) //指示在等待注册信息的过程中再次出现断线情况，跳出准备进行再次SOCKET连接
				{
					break;
				}
			}
			if((uxBits & REGISTERALL) == REGISTERALL) //只有等到所有注册信息回复完成之后才能进行开始数据传输
			{
				xTimerStart(DS18B20Timer_Handle,0);
				xTimerStart(InfraredTempTimer_Handle,0);
				xTimerStart(CurrentTimer_Handle,0);
				xTimerStart(VibratorTimer_Handle,0);
				xTimerStart(SwitchTimer_Handle,0);
				xTimerStart(HeartBeatTimer_Handle,0);
			}
		}
	}
}



EventBits_t Machine_Register_CMD(TickType_t pportMAX_DELAY)
{
	ReportCmd(Machine_Register,SIM7600CE_Info.ICCID_Data,20,0);
	Sense_Register(InfraredTemp_Register,INFRAREDTEMP_NUM);
	Sense_Register(Current_Register,CURRENT_NUM);
	Sense_Register(Vibrator_Register,VIBRATOR_NUM);
	SenseDS18B20_Register();
	return xEventGroupWaitBits((EventGroupHandle_t	)TCPEventGroupHandler,		
							(EventBits_t		)REGISTERALL,
							(BaseType_t			)pdTRUE,				
							(BaseType_t			)pdTRUE,
							(TickType_t			)pportMAX_DELAY);
}

uint8_t ReportCmd(ReportCode_TypeDef function,uint8_t *pBuffer,uint8_t len,uint8_t errror)
{
	uint16_t CrcData;
	uint8_t i,QueueSend;
	uint8_t *pSendBuffer;
	/***********************************************************************/
	QueueSend = uxQueueSpacesAvailable(SendMessage_Queue);//得到队列剩余大小
	if(QueueSend != 0)
	{
		SendQueue_Count = SendQueue_Count % BUFFER_DEPTH;
		memset(Send_Buffer[SendQueue_Count],0,SIM7600CE_BUFFLEN);
		if((function < 0x80)||(function == HeartBeat_Frame))
		{
			Send_Buffer[SendQueue_Count][0] = len+11;
			Send_Buffer[SendQueue_Count][1] = Machine_Addr >> 8;
			Send_Buffer[SendQueue_Count][2] = Machine_Addr;
			Send_Buffer[SendQueue_Count][3] = function;
			Send_Buffer[SendQueue_Count][4] = len+7;
			Send_Buffer[SendQueue_Count][5] = Machine_REV;
			for(i=0;i<len;i++)
			{
				Send_Buffer[SendQueue_Count][6+i] = pBuffer[i];
			}
			CrcData = MBCrcCheck(&Send_Buffer[SendQueue_Count][1],len+5);
			Send_Buffer[SendQueue_Count][len+6] = CrcData >> 8;
			Send_Buffer[SendQueue_Count][len+7] = CrcData;
			for(i=0;i<4;i++)
			{
				Send_Buffer[SendQueue_Count][len+8+i] = 0xFE;
			}
		}
		else
		{
			Send_Buffer[SendQueue_Count][0] = len+12;
			Send_Buffer[SendQueue_Count][1] = Machine_Addr >> 8;
			Send_Buffer[SendQueue_Count][2] = Machine_Addr;
			Send_Buffer[SendQueue_Count][3] = 0xFE;
			Send_Buffer[SendQueue_Count][4] = len+8;
			Send_Buffer[SendQueue_Count][5] = function;
			Send_Buffer[SendQueue_Count][6] = errror;
			for(i=0;i<len;i++)
			{
				Send_Buffer[SendQueue_Count][7+i] = pBuffer[i];
			}
			CrcData = MBCrcCheck(&Send_Buffer[SendQueue_Count][1],len+6);
			Send_Buffer[SendQueue_Count][len+7] = CrcData >> 8;
			Send_Buffer[SendQueue_Count][len+8] = CrcData;
			for(i=0;i<4;i++)
			{
				Send_Buffer[SendQueue_Count][len+9+i] = 0xFE;
			}
		}
		pSendBuffer = Send_Buffer[SendQueue_Count];
		xQueueSend(SendMessage_Queue,&pSendBuffer,portMAX_DELAY);
		SendQueue_Count++;
		return 0;
	}
	else
	{
		return 1;
	}
}

void ReceSperate(uint8_t *pBuffer,uint8_t len,uint8_t *pSperateNum)
{
	uint8_t i,j,count,pSperateCount[10],pSperateLen;
	*pSperateNum = 0;
	pSperateCount[0] = 0;
	for(i=0;i<len;i++)
	{
		if(pBuffer[i] == 0xFE)
		{
			count = 0;
			for(j=i;j<i+4;j++)
			{
				if(pBuffer[j] == 0xFE)
				{
					count++;
				}
			}
			if(count == 4)
			{
				*pSperateNum = *pSperateNum +1;
				i=i+4;
				pSperateCount[*pSperateNum] = i;
			}
		}
	}
	if(*pSperateNum == 1)
	{
		ReceCmd(pBuffer,len);
	}
	else
	{
		for(i=0;i<*pSperateNum;i++)
		{
			pSperateLen = pSperateCount[i + 1] - pSperateCount[i];
			ReceCmd(&pBuffer[pSperateCount[i]],pSperateLen);
		}
	}
}

uint8_t ReceCmd(uint8_t *pBuffer,uint8_t len) 
{
	uint16_t Addrtemp;
	if(len > 6)
	{
		if(ReceCrcCheck(pBuffer,len-4))
		{
			Addrtemp = (pBuffer[0] << 8) | pBuffer[1];
			if(Addrtemp == Machine_Addr)
			{
				if((pBuffer[5] == 0x00)&&(pBuffer[2] == 0xFF))
				{
					switch(pBuffer[4])
					{
						case Machine_Register:
							xEventGroupSetBits(TCPEventGroupHandler,MACHINE_REGISTERBIT);
							break;
						case DS18B20_Register:
							xEventGroupSetBits(TCPEventGroupHandler,DS18B20_REGISTERBIT);
							break;
						case DS18B20_Data:
							xEventGroupSetBits(TCPEventGroupHandler,DS18B20_DATABIT);
							break;
						case InfraredTemp_Register:
							xEventGroupSetBits(TCPEventGroupHandler,INFRAREDTEMP_REGISTERBIT);
							break;
						case InfraredTemp_Data:
							xEventGroupSetBits(TCPEventGroupHandler,INFRAREDTEMP_DATABIT);
							break;
						case Current_Register:
							xEventGroupSetBits(TCPEventGroupHandler,CURRENT_REGISTERBTI);
							break;
						case Current_Data:
							xEventGroupSetBits(TCPEventGroupHandler,CURRENT_DATABIT);
							break;
						case Vibrator_Register:
							xEventGroupSetBits(TCPEventGroupHandler,VIBRATOR_REGISTERBIT);
							break;
						case Vibrator_Data:
							xEventGroupSetBits(TCPEventGroupHandler,VIBRATOR_DATABIT);
							break;
						case Switch_Data:
							xEventGroupSetBits(TCPEventGroupHandler,SWITCH_DATABIT);
							break;
						case Error_Code:
							xEventGroupSetBits(TCPEventGroupHandler,ERROR_CODEBIT);
							break;
					}
					return 1;
				}
				else if(pBuffer[2] == 0xE0)
				{
					xEventGroupSetBits(TCPEventGroupHandler,HEARTBEATBIT);
					return 1;
				}
				else if(pBuffer[2] & (0x80))
				{
					switch(pBuffer[2])
					{
						case Cmd_ChangeTime:
							ChangeTimeExecute(&pBuffer[5],&pBuffer[15]);
							break;
						case Cmd_SwitchOutput:
							SwitchOutputExecute(&pBuffer[5],&pBuffer[10]);
							break;
						case Cmd_MachineUserTime:
							break;
					}
					return 1;
				}
			}
		}
	}
	return 0;
}

static void ChangeTimeExecute(uint8_t *pDatabuffer,uint8_t *pTimeBuffer)
{
	uint32_t TimeTemp;
	uint8_t sendtimetemp[2];
	uint8_t error;
	TimeTemp = ((pDatabuffer[0]<<8) | pDatabuffer[1]) * 1000;
	sendtimetemp[0] = pDatabuffer[0]<<8;
	sendtimetemp[1] = pDatabuffer[1];
	if(EEPROM_Change_Addr(EEPROM_DS18B20_SendTime,sendtimetemp,2))
	{
		xTimerChangePeriod(DS18B20Timer_Handle,TimeTemp,100);
		error = 0;
	}
	else
		error = 0xFF;
	TimeTemp = ((pDatabuffer[2]<<8) | pDatabuffer[3]) * 1000;
	sendtimetemp[0] = pDatabuffer[2]<<8;
	sendtimetemp[1] = pDatabuffer[3];
	if(EEPROM_Change_Addr(EEPROM_InfraredTemp_SendTime,sendtimetemp,2))
	{
		xTimerChangePeriod(InfraredTempTimer_Handle,TimeTemp,100);
		error = 0;
	}
	else
		error = 0xFF;
	TimeTemp = ((pDatabuffer[4]<<8) | pDatabuffer[5]) * 1000;
	sendtimetemp[0] = pDatabuffer[4]<<8;
	sendtimetemp[1] = pDatabuffer[5];
	if(EEPROM_Change_Addr(EEPROM_Current_SendTime,sendtimetemp,2))
	{
		xTimerChangePeriod(CurrentTimer_Handle,TimeTemp,100);
		error = 0;
	}
	else
		error = 0xFF;
	TimeTemp = ((pDatabuffer[6]<<8) | pDatabuffer[7]) * 1000;
	sendtimetemp[0] = pDatabuffer[6]<<8;
	sendtimetemp[1] = pDatabuffer[7];
	if(EEPROM_Change_Addr(EEPROM_Vibrator_SendTime,sendtimetemp,2))
	{
		xTimerChangePeriod(VibratorTimer_Handle,TimeTemp,100);
		error = 0;
	}
	else
		error = 0xFF;
	TimeTemp = ((pDatabuffer[8]<<8) | pDatabuffer[9]) * 1000;
	sendtimetemp[0] = pDatabuffer[8]<<8;
	sendtimetemp[1] = pDatabuffer[9];
	if(EEPROM_Change_Addr(EEPROM_Switch_SendTime,sendtimetemp,2))
	{
		xTimerChangePeriod(SwitchTimer_Handle,TimeTemp,100);
		error = 0;
	}
	else
		error = 0xFF;
	ReportCmd(Cmd_ChangeTime,pTimeBuffer,6,error);
}

static void SwitchOutputExecute(uint8_t *pDatabuffer,uint8_t *pTimeBuffer)
{
	if(pDatabuffer[0])
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_OUTPUT1_BIT);
	else
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_OUTPUT1_BIT);
	if(pDatabuffer[1])
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_OUTPUT2_BIT);
	else
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_OUTPUT2_BIT);
	if(pDatabuffer[2])
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_OUTPUT3_BIT);
	else
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_OUTPUT3_BIT);
	if(pDatabuffer[3])
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_OUTPUT4_BIT);
	else
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_OUTPUT4_BIT);
	if(pDatabuffer[4])
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_OUTPUT5_BIT);
	else
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_OUTPUT5_BIT);
	ReportCmd(Cmd_SwitchOutput,pTimeBuffer,6,0);
}

static void Sense_Register(ReportCode_TypeDef function,uint8_t pOnlineNum)
{
	uint8_t TempBuffer[50],i;
	TempBuffer[0] = pOnlineNum;
	switch(function)
	{
		case InfraredTemp_Register:
			for(i=0;i<pOnlineNum;i++)
			{
				TempBuffer[1+i*4] = Sense_Data.InfraredTemp[i].Addr >> 24;
				TempBuffer[2+i*4] = Sense_Data.InfraredTemp[i].Addr >> 16;
				TempBuffer[3+i*4] = Sense_Data.InfraredTemp[i].Addr >> 8;
				TempBuffer[4+i*4] = Sense_Data.InfraredTemp[i].Addr;
			}
			ReportCmd(InfraredTemp_Register,TempBuffer,pOnlineNum*4+1,0);
			break;
		case Current_Register:
			for(i=0;i<pOnlineNum;i++)
			{
				TempBuffer[1+i*4] = Sense_Data.Current[i].Addr >> 24;
				TempBuffer[2+i*4] = Sense_Data.Current[i].Addr >> 16;
				TempBuffer[3+i*4] = Sense_Data.Current[i].Addr >> 8;
				TempBuffer[4+i*4] = Sense_Data.Current[i].Addr;
			}
			ReportCmd(Current_Register,TempBuffer,pOnlineNum*4+1,0);
			break;
		case Vibrator_Register:
			for(i=0;i<pOnlineNum;i++)
			{
				TempBuffer[1+i*4] = Sense_Data.Vibrator[i].Addr >> 24;
				TempBuffer[2+i*4] = Sense_Data.Vibrator[i].Addr >> 16;
				TempBuffer[3+i*4] = Sense_Data.Vibrator[i].Addr >> 8;
				TempBuffer[4+i*4] = Sense_Data.Vibrator[i].Addr;
			}
			ReportCmd(Vibrator_Register,TempBuffer,pOnlineNum*4+1,0);
			break;
	}
}

static void SenseDS18B20_Register(void)
{
	uint8_t TempBuffer[128],i;
	TempBuffer[0] = DS18B20Temp.SetNum;
	for(i=0;i<DS18B20Temp.SetNum;i++)
	{
		TempBuffer[1+i*6] = DS18B20Temp.TempSense[i].ID[1];
		TempBuffer[2+i*6] = DS18B20Temp.TempSense[i].ID[2];
		TempBuffer[3+i*6] = DS18B20Temp.TempSense[i].ID[3];
		TempBuffer[4+i*6] = DS18B20Temp.TempSense[i].ID[4];
		TempBuffer[5+i*6] = DS18B20Temp.TempSense[i].ID[5];
		TempBuffer[6+i*6] = DS18B20Temp.TempSense[i].ID[6];
	}
	ReportCmd(DS18B20_Register,TempBuffer,DS18B20Temp.SetNum*6+1,0);
}

