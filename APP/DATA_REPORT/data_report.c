
#include "data_report.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"

#include "sense_modbus.h"
#include "DS18B20_GetTemperature.h"
#include "communication_tcp.h"
#include "Switch_Task.h"
#include "rtc.h"

extern void File_Add_Data(_calendar_obj *pCalendar,ReportCode_TypeDef function);

#define DS18B20_DATABIT            (1<<3)
#define INFRAREDTEMP_DATABIT       (1<<5)
#define CURRENT_DATABIT            (1<<7)
#define VIBRATOR_DATABIT           (1<<9)
#define SWITCH_DATABIT             (1<<10)
#define HEARTBEATBIT               (1<<12)

extern EventGroupHandle_t TCPEventGroupHandler;	//事件标志组句柄

extern EventGroupHandle_t SwitchEventGroupHandler;	//事件标志组句柄

EventBits_t EventValue;

uint8_t DataReportBuffer[50];

void DS18B20TimerCallback(TimerHandle_t xTimer)
{
	uint8_t i,len;
	EventValue = xEventGroupGetBits(TCPEventGroupHandler);
	xEventGroupClearBits(TCPEventGroupHandler,DS18B20_DATABIT);
	for(i=0;i<MAXNUM;i++)
	{
		DataReportBuffer[0+i*2] = DS18B20Temp.TempSense[i].Data >> 8;
		DataReportBuffer[1+i*2] = DS18B20Temp.TempSense[i].Data;
	}
	len = MAXNUM * 2;
	ReportCmd(DS18B20_Data,DataReportBuffer,len,0);
	File_Add_Data(&calendar,DS18B20_Data);
}
void InfraredTempTimerCallback(TimerHandle_t xTimer)
{
	uint8_t i,len;
	EventValue = xEventGroupGetBits(TCPEventGroupHandler);
	xEventGroupClearBits(TCPEventGroupHandler,INFRAREDTEMP_DATABIT);
	for(i=0;i<INFRAREDTEMP_NUM;i++)
	{
		DataReportBuffer[0+i*2] = Sense_Data.InfraredTemp[i].Data >> 8;
		DataReportBuffer[1+i*2] = Sense_Data.InfraredTemp[i].Data;
	}
	len = INFRAREDTEMP_NUM * 2;
	ReportCmd(InfraredTemp_Data,DataReportBuffer,len,0);
	File_Add_Data(&calendar,InfraredTemp_Data);
}
void CurrentTimerCallback(TimerHandle_t xTimer)
{
	uint8_t i,len;
	EventValue = xEventGroupGetBits(TCPEventGroupHandler);
	xEventGroupClearBits(TCPEventGroupHandler,CURRENT_DATABIT);
	for(i=0;i<CURRENT_NUM;i++)
	{
		DataReportBuffer[0+i*2] = Sense_Data.Current[i].Data >> 8;
		DataReportBuffer[1+i*2] = Sense_Data.Current[i].Data;
	}
	len = CURRENT_NUM * 2;
	ReportCmd(Current_Data,DataReportBuffer,len,0);
	File_Add_Data(&calendar,Current_Data);
}
void VibratorTimerCallback(TimerHandle_t xTimer)
{
	uint8_t i,len;
	EventValue = xEventGroupGetBits(TCPEventGroupHandler);
	xEventGroupClearBits(TCPEventGroupHandler,VIBRATOR_DATABIT);
	for(i=0;i<VIBRATOR_NUM;i++)
	{
		DataReportBuffer[0+i*7] = VIBRATOR_AVERFUNCTION;
		DataReportBuffer[1+i*7] = Sense_Data.Vibrator[i].AverData[x] >> 8;
		DataReportBuffer[2+i*7] = Sense_Data.Vibrator[i].AverData[x];
		DataReportBuffer[3+i*7] = Sense_Data.Vibrator[i].AverData[y] >> 8;
		DataReportBuffer[4+i*7] = Sense_Data.Vibrator[i].AverData[y];
		DataReportBuffer[5+i*7] = Sense_Data.Vibrator[i].AverData[z] >> 8;
		DataReportBuffer[6+i*7] = Sense_Data.Vibrator[i].AverData[z];
	}
	for(i=0;i<VIBRATOR_NUM;i++)
	{
		DataReportBuffer[VIBRATOR_NUM*7+1+i*7] = VIBRATOR_MAXFUNCTION;
		DataReportBuffer[VIBRATOR_NUM*7+2+i*7] = Sense_Data.Vibrator[i].MaxData[x] >> 8;
		DataReportBuffer[VIBRATOR_NUM*7+3+i*7] = Sense_Data.Vibrator[i].MaxData[x];
		DataReportBuffer[VIBRATOR_NUM*7+4+i*7] = Sense_Data.Vibrator[i].MaxData[y] >> 8;
		DataReportBuffer[VIBRATOR_NUM*7+5+i*7] = Sense_Data.Vibrator[i].MaxData[y];
		DataReportBuffer[VIBRATOR_NUM*7+6+i*7] = Sense_Data.Vibrator[i].MaxData[z] >> 8;
		DataReportBuffer[VIBRATOR_NUM*7+7+i*7] = Sense_Data.Vibrator[i].MaxData[z];
	}
	len = VIBRATOR_NUM * 14;
	ReportCmd(Vibrator_Data,DataReportBuffer,len,0);
	File_Add_Data(&calendar,Vibrator_Data);
	
}
void SwitchTimerCallback(TimerHandle_t xTimer)
{
	EventBits_t OutputEventValue;
	uint8_t i,len;
	
	EventValue = xEventGroupGetBits(TCPEventGroupHandler);
	xEventGroupClearBits(TCPEventGroupHandler,SWITCH_DATABIT);
	OutputEventValue = xEventGroupGetBits(SwitchEventGroupHandler);
	DataReportBuffer[0] = SWITCH_INPUT_NUM;
	DataReportBuffer[1] = SWITCH_OUTPUT_NUM;
	for(i=0;i<(SWITCH_OUTPUT_NUM+SWITCH_INPUT_NUM);i++)
	{
		if(OutputEventValue&(1<<i))
		{
			DataReportBuffer[2+i] = 1;
		}
		else
		{
			DataReportBuffer[2+i] = 0;
		}
	}
	len = SWITCH_OUTPUT_NUM+SWITCH_INPUT_NUM+2;
	ReportCmd(Switch_Data,DataReportBuffer,len,0);
	File_Add_Data(&calendar,Switch_Data);
}
void HeartBeatTimerCallback(TimerHandle_t xTimer)
{
	EventValue = xEventGroupGetBits(TCPEventGroupHandler);
	xEventGroupClearBits(TCPEventGroupHandler,HEARTBEATBIT);
	ReportCmd(HeartBeat_Frame,NULL,0,0);
}

