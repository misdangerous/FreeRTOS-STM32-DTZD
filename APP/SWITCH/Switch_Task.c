
#include "Switch_Task.h"
#include "switch_io.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"

struct Switch_Struct Switch_Struction;

EventGroupHandle_t SwitchEventGroupHandler;	//事件标志组句柄

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


#define SWITCH_OUTPUT1_BIT           (1<<7)
#define SWITCH_OUTPUT2_BIT           (1<<8)
#define SWITCH_OUTPUT3_BIT           (1<<9)
#define SWITCH_OUTPUT4_BIT           (1<<10)
#define SWITCH_OUTPUT5_BIT           (1<<11)

static void OutputExecute(void);
static void InputExecute(void);

void vSwitchTask( void *pvParameters )
{
	SwitchEventGroupHandler=xEventGroupCreate();	 //创建事件标志组
	xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_OUTPUT2_BIT);
	while(1)
	{
		OutputExecute();
		InputExecute();
		vTaskDelay(500);
	}
}

static void OutputExecute(void)
{
	EventBits_t OutputEventValue;
	OutputEventValue = xEventGroupGetBits(SwitchEventGroupHandler);
	if(OutputEventValue & SWITCH_OUTPUT1_BIT)
	{
		SWITCH_OUTPUT_1 = OUTPUT_ON;
		Switch_Struction.OUT[0] = Switch_ON;
	}
	else
	{
		SWITCH_OUTPUT_1 = OUTPUT_OFF;
		Switch_Struction.OUT[0] = Switch_OFF;
	}
	if(OutputEventValue & SWITCH_OUTPUT2_BIT)
	{
		SWITCH_OUTPUT_2 = OUTPUT_ON;
		Switch_Struction.OUT[1] = Switch_ON;
	}
	else
	{
		SWITCH_OUTPUT_2 = OUTPUT_OFF;
		Switch_Struction.OUT[1] = Switch_OFF;
	}
	if(OutputEventValue & SWITCH_OUTPUT3_BIT)
	{
		SWITCH_OUTPUT_3 = OUTPUT_ON;
		Switch_Struction.OUT[2] = Switch_ON;
	}
	else
	{
		SWITCH_OUTPUT_3 = OUTPUT_OFF;
		Switch_Struction.OUT[2] = Switch_OFF;
	}
	if(OutputEventValue & SWITCH_OUTPUT4_BIT)
	{
		SWITCH_OUTPUT_4 = OUTPUT_ON;
		Switch_Struction.OUT[3] = Switch_ON;
	}
	else
	{
		SWITCH_OUTPUT_4 = OUTPUT_OFF;
		Switch_Struction.OUT[3] = Switch_OFF;
	}
	if(OutputEventValue & SWITCH_OUTPUT5_BIT)
	{
		SWITCH_OUTPUT_5 = OUTPUT_ON;
		Switch_Struction.OUT[4] = Switch_ON;
	}
	else
	{
		SWITCH_OUTPUT_5 = OUTPUT_OFF;
		Switch_Struction.OUT[4] = Switch_OFF;
	}
}
static void InputExecute(void)
{
	if(SWITCH_INPUT_1 == INPUT_ON)
	{
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_INPUT1_BIT);
		Switch_Struction.IN[0] = Switch_ON;
	}
	else
	{
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_INPUT1_BIT);
		Switch_Struction.IN[0] = Switch_OFF;
	}
	if(SWITCH_INPUT_2 == INPUT_ON)
	{
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_INPUT2_BIT);
		Switch_Struction.IN[1] = Switch_ON;
	}
	else
	{
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_INPUT2_BIT);
		Switch_Struction.IN[1] = Switch_OFF;
	}
	if(SWITCH_INPUT_3 == INPUT_ON)
	{
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_INPUT3_BIT);
		Switch_Struction.IN[2] = Switch_ON;
	}
	else
	{
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_INPUT3_BIT);
		Switch_Struction.IN[2] = Switch_OFF;
	}
	if(SWITCH_INPUT_4 == INPUT_ON)
	{
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_INPUT4_BIT);
		Switch_Struction.IN[3] = Switch_ON;
	}
	else
	{
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_INPUT4_BIT);
		Switch_Struction.IN[3] = Switch_OFF;
	}
	if(SWITCH_INPUT_5 == INPUT_ON)
	{
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_INPUT5_BIT);
		Switch_Struction.IN[4] = Switch_ON;
	}
	else
	{
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_INPUT5_BIT);
		Switch_Struction.IN[4] = Switch_OFF;
	}
	if(SWITCH_INPUT_6 == INPUT_ON)
	{
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_INPUT6_BIT);
		Switch_Struction.IN[5] = Switch_ON;
	}
	else
	{
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_INPUT6_BIT);
		Switch_Struction.IN[5] = Switch_OFF;
	}
	if(SWITCH_INPUT_7 == INPUT_ON)
	{
		xEventGroupSetBits(SwitchEventGroupHandler,SWITCH_INPUT7_BIT);
		Switch_Struction.IN[6] = Switch_ON;
	}
	else
	{
		xEventGroupClearBits(SwitchEventGroupHandler,SWITCH_INPUT7_BIT);
		Switch_Struction.IN[6] = Switch_OFF;
	}
}
