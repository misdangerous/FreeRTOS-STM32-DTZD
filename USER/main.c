
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "switch_io.h"
#include "DS18B20_GetTemperature.h"
#include "sense_modbus.h"
#include "eeprom_save.h"
#include "sim7600ce_tcp.h"
#include "communication_tcp.h"
#include "Switch_Task.h"
#include "sim7600ce.h"
#include "rtc.h"
#include "at24cxx.h"
/*
 * Register the generic commands that can be used with FreeRTOS+CLI.
 */
extern void vRegisterSampleCLICommands( void );

/*
 * Register the file system commands that can be used with FreeRTOS+CLI.
 */
extern void vRegisterFileSystemCLICommands( void );



extern void vUARTCommandInterpreterTask( void *pvParameters );

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);



//任务优先级
#define UARTCommand_TASK_PRIO		4
//任务堆栈大小	
#define UARTCommand_STK_SIZE 		512  
//任务句柄
TaskHandle_t UARTCommand_Task_Handler;
//任务函数
void vUARTCommandInterpreterTask(void *pvParameters);

//任务优先级
#define DS18B20GetTemperature_TASK_PRIO		5
//任务堆栈大小	
#define DS18B20GetTemperature_STK_SIZE 		512  
//任务句柄
TaskHandle_t DS18B20GetTemperature_Task_Handler;
//任务函数
void vDS18B20GetTemperatureTask( void *pvParameters );

//任务优先级
#define Sense_Modbus_TASK_PRIO		6
//任务堆栈大小	
#define Sense_Modbus_STK_SIZE 		128  
//任务句柄
TaskHandle_t Sense_Modbus_Task_Handler;
//任务函数
void vSense_ModbusTask( void *pvParameters );

//任务优先级
#define SIM7600CE_TCP_TASK_PRIO		9
//任务堆栈大小	
#define SIM7600CE_TCP_STK_SIZE 		256  
//任务句柄
TaskHandle_t SIM7600CE_TCP_Task_Handler;
//任务函数
void vSIM7600CE_TCPTask( void *pvParameters );

//任务优先级
#define TCP_Rece_TASK_PRIO		8
//任务堆栈大小	
#define TCP_Rece_STK_SIZE 		128  
//任务句柄
TaskHandle_t TCP_Rece_Task_Handler;
//任务函数
void vTCP_Rece_Task( void *pvParameters );

//任务优先级
#define TCP_ReceComm_TASK_PRIO		10
//任务堆栈大小	
#define TCP_ReceComm_STK_SIZE 		128  
//任务句柄
TaskHandle_t TCP_ReceComm_Task_Handler;
//任务函数
void vTCP_ReceCommTask( void *pvParameters );

//任务优先级
#define TCP_SendComm_TASK_PRIO		11
//任务堆栈大小	
#define TCP_SendComm_STK_SIZE 		128  
//任务句柄
TaskHandle_t TCP_SendComm_Task_Handler;
//任务函数
void vTCP_SendCommTask( void *pvParameters );

//任务优先级
#define Switch_TASK_PRIO		12
//任务堆栈大小	
#define Switch_STK_SIZE 		52
//任务句柄
TaskHandle_t Switch_Task_Handler;
//任务函数
void vSwitchTask( void *pvParameters );

//任务优先级
#define TCP_ReconnectSocke_TASK_PRIO		13
//任务堆栈大小	
#define TCP_ReconnectSocke_STK_SIZE 		128  
//任务句柄
TaskHandle_t TCP_ReconnectSocke_Task_Handler;
//任务函数
void vTCP_ReconnectSockeTask( void *pvParameters );


//按键消息队列的数量
#define MESSAGE_Q_NUM   4   	//发送数据的消息队列的数量 
QueueHandle_t Message_Queue;	//信息队列句柄

//二值信号量句柄
SemaphoreHandle_t BinarySemaphore;	//二值信号量句柄


QueueHandle_t SendMessage_Queue;	//信息队列句柄
QueueHandle_t ReceMessage_Queue;	//信息队列句柄

EventGroupHandle_t TCPEventGroupHandler;	//事件标志组句柄

SemaphoreHandle_t EEPROMBinarySemaphore;	//二值信号量句柄

uint16_t Machine_Addr = Machine_Addr_Default;

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	
	RTC_Init();	
	delay_init();	    				//延时函数初始化	  
	uart_init(9600);					//初始化串口
	LED_Init();		  					//初始化LED
	SwitchIO_Init();
	I2C_InitCfg();
	/* Register generic commands with the FreeRTOS+CLI command interpreter. */
	vRegisterSampleCLICommands();

	/* Register file system related commands with the FreeRTOS+CLI command
	interpreter. */
	vRegisterFileSystemCLICommands();
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}


//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
	Message_Queue=xQueueCreate(MESSAGE_Q_NUM,USART_REC_LEN); //创建消息Message_Queue,队列项长度是串口接收缓冲区长度
	BinarySemaphore=xSemaphoreCreateBinary();
	xSemaphoreGive(BinarySemaphore);	//释放二值信号量
	TCPEventGroupHandler=xEventGroupCreate();	 //创建事件标志组
	SendMessage_Queue=xQueueCreate(BUFFER_DEPTH,sizeof(uint8_t *)); //创建消息Message_Queue,队列项长度是串口接收缓冲区长度
	ReceMessage_Queue=xQueueCreate(BUFFER_DEPTH,sizeof(uint8_t *)); //创建消息Message_Queue,队列项长度是串口接收缓冲区长度
	
	EEPROMBinarySemaphore=xSemaphoreCreateBinary();
	xSemaphoreGive(EEPROMBinarySemaphore);	//释放二值信号量
	InitMachineAddr(&Machine_Addr);
	
    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
	//创建UartCommand任务
    xTaskCreate((TaskFunction_t )vUARTCommandInterpreterTask,     
                (const char*    )"UARTCommand_task",   
                (uint16_t       )UARTCommand_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )UARTCommand_TASK_PRIO,
                (TaskHandle_t*  )&UARTCommand_Task_Handler); 		
	//创建DS18B20任务
    xTaskCreate((TaskFunction_t )vDS18B20GetTemperatureTask,     
                (const char*    )"DS18B20Get_task",   
                (uint16_t       )DS18B20GetTemperature_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )DS18B20GetTemperature_TASK_PRIO,
                (TaskHandle_t*  )&DS18B20GetTemperature_Task_Handler); 	
	//创建Sense_Modbus任务
    xTaskCreate((TaskFunction_t )vSense_ModbusTask,     
                (const char*    )"Sense_Modbus_task",   
                (uint16_t       )Sense_Modbus_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )Sense_Modbus_TASK_PRIO,
                (TaskHandle_t*  )&Sense_Modbus_Task_Handler); 
	//创建EEPROM_Save任务
    xTaskCreate((TaskFunction_t )vSIM7600CE_TCPTask,     
                (const char*    )"SIM7600CE_TCP_task",   
                (uint16_t       )SIM7600CE_TCP_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )SIM7600CE_TCP_TASK_PRIO,
                (TaskHandle_t*  )&SIM7600CE_TCP_Task_Handler);
	xTaskCreate((TaskFunction_t )vTCP_Rece_Task,     
                (const char*    )"TCP_Rece_Task",   
                (uint16_t       )TCP_Rece_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TCP_Rece_TASK_PRIO,
                (TaskHandle_t*  )&TCP_Rece_Task_Handler);
    xTaskCreate((TaskFunction_t )vTCP_ReceCommTask,     
                (const char*    )"TCP_ReceComm_Task",   
                (uint16_t       )TCP_ReceComm_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TCP_ReceComm_TASK_PRIO,
                (TaskHandle_t*  )&TCP_ReceComm_Task_Handler);	
    xTaskCreate((TaskFunction_t )vTCP_SendCommTask,     
                (const char*    )"TCP_SendComm_Task",   
                (uint16_t       )TCP_SendComm_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TCP_SendComm_TASK_PRIO,
                (TaskHandle_t*  )&TCP_SendComm_Task_Handler);
	xTaskCreate((TaskFunction_t )vSwitchTask,     
                (const char*    )"Switch_Task",   
                (uint16_t       )Switch_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )Switch_TASK_PRIO,
                (TaskHandle_t*  )&Switch_Task_Handler);	
	xTaskCreate((TaskFunction_t )vTCP_ReconnectSockeTask,     
                (const char*    )"TCP_Reconnect_Task",   
                (uint16_t       )TCP_ReconnectSocke_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TCP_ReconnectSocke_TASK_PRIO,
                (TaskHandle_t*  )&TCP_ReconnectSocke_Task_Handler);					
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//LED0任务函数 
void led0_task(void *pvParameters)
{
    while(1)
    {
        LED0=~LED0;
        vTaskDelay(1000);
    }
}   

