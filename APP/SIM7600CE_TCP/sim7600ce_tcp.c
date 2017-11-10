#include "sim7600ce_tcp.h"
#include "sim7600ce.h"
#include "communication_tcp.h"
#include "eeprom_save.h"

#include "DS18B20_GetTemperature.h"
#include "sense_modbus.h"


#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "timers.h"

char *SIM7600CE_InitOK={"PB DONE"};

char *ECHOOFF_CMD = {"ATE0\r\n"};
char *CICCID_CMD = {"AT+CICCID\r\n"};
char *CGSOCKCONT_CMD = {"AT+CGSOCKCONT=1,\"IP\",\"CMNET\"\r\n"};
char *CSOCKSETPN_CMD = {"AT+CSOCKSETPN=1\r\n"};
char *CIPMODE_CMD = {"AT+CIPMODE=1\r\n"};
char *NETOPEN_CMD = {"AT+NETOPEN\r\n"};
char *NETOPEN_OK = {"+NETOPEN: 0"};
//char *CIPOPEN_CMD = {"AT+CIPOPEN=0,\"TCP\",\"wq1993316.vicp.io\",19730\r\n"};
char *CIPOPEN_CMD = {"AT+CIPOPEN=0,\"TCP\",\"112.33.12.121\",8001\r\n"};
char *OK_CMD = {"OK"};
char *CONNECT_CMD = {"CONNECT 115200"};
char *CLOSE_CMD = {"CLOSED\r\n"};
char *ICCID_HEADER_CMD = {"+ICCID: "};

char *NETTEST_CMD = {"AT+NETOPEN?\r\n"};

/*4G模块接收缓冲区*/
uint8_t SIM7600CE_Buffer[SIM7600CE_BUFFLEN];
/*存储4G模块信息*/
struct SIM7600CE_Info SIM7600CE_Info;

/*缓冲区的0地址为长度*/
uint8_t Rece_Buffer[BUFFER_DEPTH][SIM7600CE_BUFFLEN];
uint8_t ReceQueue_Count=0;

extern QueueHandle_t SendMessage_Queue;	//信息队列句柄
extern QueueHandle_t ReceMessage_Queue;	//信息队列句柄

extern EventGroupHandle_t TCPEventGroupHandler;	//事件标志组句柄
#define TCPSTARTBIT	(1<<0)				//事件位


void SetSocket(TickType_t pportMAX_DELAY);
void SIM7600CE_InitCfg(void);


TimerHandle_t 	DS18B20Timer_Handle;			
TimerHandle_t	InfraredTempTimer_Handle;		
TimerHandle_t	CurrentTimer_Handle;		    
TimerHandle_t	VibratorTimer_Handle;		    
TimerHandle_t	SwitchTimer_Handle;		       
TimerHandle_t	HeartBeatTimer_Handle;	

extern void DS18B20TimerCallback(TimerHandle_t xTimer); 	        //周期定时器回调函数
extern void InfraredTempTimerCallback(TimerHandle_t xTimer);		//单次定时器回调函数
extern void CurrentTimerCallback(TimerHandle_t xTimer); 	        //周期定时器回调函数
extern void VibratorTimerCallback(TimerHandle_t xTimer);	    	//单次定时器回调函数
extern void SwitchTimerCallback(TimerHandle_t xTimer);				//单次定时器回调函数
extern void HeartBeatTimerCallback(TimerHandle_t xTimer);  		//单次定时器回调函数


void vSIM7600CE_TCPTask( void *pvParameters )
{
	BaseType_t err=pdFALSE;
	uint32_t pSendBuffer;
	uint8_t *psendbuff;
	uint8_t len,i;
	uint8_t temp[2];
	uint32_t temp1[6];
	for(i=0;i<6;i++)
	{
		if(EEPROM_Get_Addr((EEPROM_Save_TypeDef)(EEPROM_DS18B20_SendTime+i),temp,&len))
		{
			temp1[i] = temp[0]<<8 | temp[1];
			temp1[i] = temp1[i] * 1000;
		}
		else
		{
			temp1[i] = DEFAULT_REPORTTIME_HEARTBEAT;
		}
		
	}
	DS18B20Timer_Handle = xTimerCreate((const char*		)"DS18B20Timer",
									    (TickType_t			)temp1[0],
							            (UBaseType_t		)pdTRUE,
							            (void*				)1,
							            (TimerCallbackFunction_t)DS18B20TimerCallback);
	InfraredTempTimer_Handle=xTimerCreate((const char*		)"InfraredTempTimer",
									    (TickType_t			)temp1[2],
							            (UBaseType_t		)pdTRUE,
							            (void*				)2,
							            (TimerCallbackFunction_t)InfraredTempTimerCallback);
	CurrentTimer_Handle=xTimerCreate((const char*		)"CurrentTimer",
									    (TickType_t			)temp1[1],
							            (UBaseType_t		)pdTRUE,
							            (void*				)3,
							            (TimerCallbackFunction_t)CurrentTimerCallback);
	VibratorTimer_Handle=xTimerCreate((const char*		)"VibratorTimer",
									    (TickType_t			)temp1[3],
							            (UBaseType_t		)pdTRUE,
							            (void*				)4,
							            (TimerCallbackFunction_t)VibratorTimerCallback);
	SwitchTimer_Handle=xTimerCreate((const char*		)"SwitchTimer",
									    (TickType_t			)temp1[4],
							            (UBaseType_t		)pdTRUE,
							            (void*				)5,
							            (TimerCallbackFunction_t)SwitchTimerCallback);
	HeartBeatTimer_Handle=xTimerCreate((const char*		)"HeartBeatTimer",
									    (TickType_t			)temp1[5],
							            (UBaseType_t		)pdTRUE,
							            (void*				)6,
							            (TimerCallbackFunction_t)HeartBeatTimerCallback);
	SIM7600CE_InitCfg();
	xEventGroupSetBits(TCPEventGroupHandler,TCPSTARTBIT);
	while(1)
	{
		err=xQueueReceive(SendMessage_Queue,&pSendBuffer,portMAX_DELAY);//请求消息Message_Queue
		if(err==pdTRUE)			//接收到消息
		{
			psendbuff = (uint8_t *)pSendBuffer;
			len = psendbuff[0];
			SIM7600CESend(psendbuff+1,len);
		}
	}
}

void vTCP_Rece_Task( void *pvParameters )
{
	uint8_t Queue_Remain;
	uint8_t *pReceBuffer;
	while(1)
	{
		Queue_Remain = uxQueueSpacesAvailable(ReceMessage_Queue);
		if(Queue_Remain != 0)
		{	
			ReceQueue_Count = ReceQueue_Count % BUFFER_DEPTH;
			SIM7600CERece(&Rece_Buffer[ReceQueue_Count][1],&Rece_Buffer[ReceQueue_Count][0],portMAX_DELAY);
			pReceBuffer = Rece_Buffer[ReceQueue_Count];
			xQueueSend(ReceMessage_Queue,&pReceBuffer,portMAX_DELAY);
			ReceQueue_Count++;
		}
		else
		{
			/*接收队列缓冲区溢出*/
		}
	}
}


void SIM7600CE_InitCfg(void)
{
	SIM7600CE_Init(115200);
	SIM7600CE_RESET();
	SIM7600CE_PWRKEY();
	WaitSIM7600CE_Init(SIM7600CE_InitOK);
	vTaskDelay(2000);
	SetSocket(portMAX_DELAY);
}

void WaitSIM7600CE_Init(char *pStrCharSuccess)
{
	BaseType_t err=pdFALSE;
	uint32_t pReceBuffer;
	uint8_t *precebuff;
	
	while(1)
	{
		err=xQueueReceive(ReceMessage_Queue,&pReceBuffer,portMAX_DELAY);//请求消息Message_Queue
		if(err==pdTRUE)			//接收到消息
		{
			precebuff = (uint8_t *)pReceBuffer;
			if(strstr((const char*)&precebuff[1],pStrCharSuccess) != NULL)
			{
				break;
			}
		}
	}
}
uint8_t CMD_Respose(char *pCMD,char *pRespose,TickType_t pportMAX_DELAY)
{
	BaseType_t err=pdFALSE;
	uint32_t pReceBuffer1;
	uint8_t *precebuff1;
	
	SIM7600CESend((uint8_t *)pCMD,strlen(pCMD));
	while(1)
	{
		err=xQueueReceive(ReceMessage_Queue,&pReceBuffer1,pportMAX_DELAY);//请求消息Message_Queue
		if(err==pdTRUE)			//接收到消息
		{
			precebuff1 = (uint8_t *)pReceBuffer1;
			if(strstr((const char*)&precebuff1[1],pRespose) != NULL)
			{
				return 1;
			}
			else
			{
				if(strstr((const char*)&precebuff1[1],"ERROR") != NULL)
				{
					return 0;
				}
			}
		}
	}
}

static void Get_ICCID(void)
{
	uint8_t i;
	char *p ;
	p=strstr((char *)&Rece_Buffer[ReceQueue_Count][1],ICCID_HEADER_CMD);
	if(p != NULL)
	{
		p = p + strlen(ICCID_HEADER_CMD);
		for(i=0;i<20;i++)
		{
			SIM7600CE_Info.ICCID_Data[i] = p[i];
		}
	}
}
void SetSocket(TickType_t pportMAX_DELAY)
{
	SIM7600CE_Info.Online = SIM7600CE_CLOSE;
	while(SIM7600CE_Info.Online == SIM7600CE_CLOSE )
	{
		switch(SIM7600CE_Info.SIM7600CE_Flag)
		{
			case ECHO_OFF:
				if(CMD_Respose(ECHOOFF_CMD,OK_CMD,pportMAX_DELAY))
				{
					SIM7600CE_Info.SIM7600CE_Flag++;
				}
				else
				{
					SIM7600CE_Info.Error = SetError_ECHO_OFF;
				}
				break;
			case CICCID:
				if(CMD_Respose(CICCID_CMD,OK_CMD,pportMAX_DELAY))
				{
					SIM7600CE_Info.SIM7600CE_Flag++;
					Get_ICCID();
				}
				else
				{
					SIM7600CE_Info.Error = Error_NoSIM;
				}
				break;
			case CGSOCKCONT:
				if(CMD_Respose(CGSOCKCONT_CMD,OK_CMD,pportMAX_DELAY))
				{
					SIM7600CE_Info.SIM7600CE_Flag++;
				}
				else
				{
					SIM7600CE_Info.Error = SetError_CGSOCKCONT;
				}
				break;
			case CSOCKSETPN:
				if(CMD_Respose(CSOCKSETPN_CMD,OK_CMD,pportMAX_DELAY))
				{
					SIM7600CE_Info.SIM7600CE_Flag++;
				}
				else
				{
					SIM7600CE_Info.Error = SetError_CSOCKSETPN;
				}
				break;
			case CIPMODE:
				if(CMD_Respose(CIPMODE_CMD,OK_CMD,pportMAX_DELAY))
				{
					SIM7600CE_Info.SIM7600CE_Flag++;
				}
				else
				{
					SIM7600CE_Info.Error = SetError_CIPMODE;
				}
				break;
			case NETOPEN:
				if(CMD_Respose(NETOPEN_CMD,NETOPEN_OK,pportMAX_DELAY))
				{
					SIM7600CE_Info.SIM7600CE_Flag++;
					//SIM7600CESend((uint8_t *)NETTEST_CMD,strlen(NETTEST_CMD));
				}
				else
				{
					SIM7600CE_Info.Error = SetError_NETOPEN;
				}
				break;
			case CIPOPEN:
				if(CMD_Respose(CIPOPEN_CMD,CONNECT_CMD,pportMAX_DELAY))
				{
					SIM7600CE_Info.Online = SIM7600CE_CONNECT;
				}
				else
				{
					SIM7600CE_Info.Error = SetError_CONNECT;
				}
				break;
				
		}
	}
}

