
#include "error_display.h"
#include "sense_485.h"
#include "Switch_Task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"
#include <string.h>

extern EventGroupHandle_t SwitchEventGroupHandler;	//�¼���־����

/*λ�Ķ�λ�����������ģ�ע�������ļ��еĶ�λҲҪͬʱ�ı�*/
#define SWITCH_INPUT1_BIT	         (1<<0)				//�¼�λ
#define SWITCH_INPUT2_BIT            (1<<1)
#define SWITCH_INPUT3_BIT            (1<<2)
#define SWITCH_INPUT4_BIT            (1<<3)
#define SWITCH_INPUT5_BIT            (1<<4)
#define SWITCH_INPUT6_BIT            (1<<5)
#define SWITCH_INPUT7_BIT            (1<<6)
//#define SWITCH_INPUT8_BIT            (1<<7)
//#define SWITCH_INPUT9_BIT            (1<<8)
//#define SWITCH_INPUT10_BIT           (1<<9)

void vErrorDisplayTask( void *pvParameters )
{
	char DisplayStr[100];
	EventBits_t SwitchEventValue;
	uint8_t i;
	uint8_t ActionNum;
	
	while(1)
	{
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
		}
		vTaskDelay(1000);
	}
}
