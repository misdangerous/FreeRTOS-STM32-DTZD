
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

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LED0_TASK_PRIO		2
//�����ջ��С	
#define LED0_STK_SIZE 		50  
//������
TaskHandle_t LED0Task_Handler;
//������
void led0_task(void *pvParameters);



//�������ȼ�
#define UARTCommand_TASK_PRIO		4
//�����ջ��С	
#define UARTCommand_STK_SIZE 		512  
//������
TaskHandle_t UARTCommand_Task_Handler;
//������
void vUARTCommandInterpreterTask(void *pvParameters);

//�������ȼ�
#define DS18B20GetTemperature_TASK_PRIO		5
//�����ջ��С	
#define DS18B20GetTemperature_STK_SIZE 		512  
//������
TaskHandle_t DS18B20GetTemperature_Task_Handler;
//������
void vDS18B20GetTemperatureTask( void *pvParameters );

//�������ȼ�
#define Sense_Modbus_TASK_PRIO		6
//�����ջ��С	
#define Sense_Modbus_STK_SIZE 		128  
//������
TaskHandle_t Sense_Modbus_Task_Handler;
//������
void vSense_ModbusTask( void *pvParameters );

//�������ȼ�
#define SIM7600CE_TCP_TASK_PRIO		9
//�����ջ��С	
#define SIM7600CE_TCP_STK_SIZE 		256  
//������
TaskHandle_t SIM7600CE_TCP_Task_Handler;
//������
void vSIM7600CE_TCPTask( void *pvParameters );

//�������ȼ�
#define TCP_Rece_TASK_PRIO		8
//�����ջ��С	
#define TCP_Rece_STK_SIZE 		128  
//������
TaskHandle_t TCP_Rece_Task_Handler;
//������
void vTCP_Rece_Task( void *pvParameters );

//�������ȼ�
#define TCP_ReceComm_TASK_PRIO		10
//�����ջ��С	
#define TCP_ReceComm_STK_SIZE 		128  
//������
TaskHandle_t TCP_ReceComm_Task_Handler;
//������
void vTCP_ReceCommTask( void *pvParameters );

//�������ȼ�
#define TCP_SendComm_TASK_PRIO		11
//�����ջ��С	
#define TCP_SendComm_STK_SIZE 		128  
//������
TaskHandle_t TCP_SendComm_Task_Handler;
//������
void vTCP_SendCommTask( void *pvParameters );

//�������ȼ�
#define Switch_TASK_PRIO		12
//�����ջ��С	
#define Switch_STK_SIZE 		52
//������
TaskHandle_t Switch_Task_Handler;
//������
void vSwitchTask( void *pvParameters );

//�������ȼ�
#define TCP_ReconnectSocke_TASK_PRIO		13
//�����ջ��С	
#define TCP_ReconnectSocke_STK_SIZE 		128  
//������
TaskHandle_t TCP_ReconnectSocke_Task_Handler;
//������
void vTCP_ReconnectSockeTask( void *pvParameters );


//������Ϣ���е�����
#define MESSAGE_Q_NUM   4   	//�������ݵ���Ϣ���е����� 
QueueHandle_t Message_Queue;	//��Ϣ���о��

//��ֵ�ź������
SemaphoreHandle_t BinarySemaphore;	//��ֵ�ź������


QueueHandle_t SendMessage_Queue;	//��Ϣ���о��
QueueHandle_t ReceMessage_Queue;	//��Ϣ���о��

EventGroupHandle_t TCPEventGroupHandler;	//�¼���־����

SemaphoreHandle_t EEPROMBinarySemaphore;	//��ֵ�ź������

uint16_t Machine_Addr = Machine_Addr_Default;

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	
	RTC_Init();	
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(9600);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
	SwitchIO_Init();
	I2C_InitCfg();
	/* Register generic commands with the FreeRTOS+CLI command interpreter. */
	vRegisterSampleCLICommands();

	/* Register file system related commands with the FreeRTOS+CLI command
	interpreter. */
	vRegisterFileSystemCLICommands();
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}


//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
	
	Message_Queue=xQueueCreate(MESSAGE_Q_NUM,USART_REC_LEN); //������ϢMessage_Queue,��������Ǵ��ڽ��ջ���������
	BinarySemaphore=xSemaphoreCreateBinary();
	xSemaphoreGive(BinarySemaphore);	//�ͷŶ�ֵ�ź���
	TCPEventGroupHandler=xEventGroupCreate();	 //�����¼���־��
	SendMessage_Queue=xQueueCreate(BUFFER_DEPTH,sizeof(uint8_t *)); //������ϢMessage_Queue,��������Ǵ��ڽ��ջ���������
	ReceMessage_Queue=xQueueCreate(BUFFER_DEPTH,sizeof(uint8_t *)); //������ϢMessage_Queue,��������Ǵ��ڽ��ջ���������
	
	EEPROMBinarySemaphore=xSemaphoreCreateBinary();
	xSemaphoreGive(EEPROMBinarySemaphore);	//�ͷŶ�ֵ�ź���
	InitMachineAddr(&Machine_Addr);
	
    //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
	//����UartCommand����
    xTaskCreate((TaskFunction_t )vUARTCommandInterpreterTask,     
                (const char*    )"UARTCommand_task",   
                (uint16_t       )UARTCommand_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )UARTCommand_TASK_PRIO,
                (TaskHandle_t*  )&UARTCommand_Task_Handler); 		
	//����DS18B20����
    xTaskCreate((TaskFunction_t )vDS18B20GetTemperatureTask,     
                (const char*    )"DS18B20Get_task",   
                (uint16_t       )DS18B20GetTemperature_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )DS18B20GetTemperature_TASK_PRIO,
                (TaskHandle_t*  )&DS18B20GetTemperature_Task_Handler); 	
	//����Sense_Modbus����
    xTaskCreate((TaskFunction_t )vSense_ModbusTask,     
                (const char*    )"Sense_Modbus_task",   
                (uint16_t       )Sense_Modbus_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )Sense_Modbus_TASK_PRIO,
                (TaskHandle_t*  )&Sense_Modbus_Task_Handler); 
	//����EEPROM_Save����
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
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LED0������ 
void led0_task(void *pvParameters)
{
    while(1)
    {
        LED0=~LED0;
        vTaskDelay(1000);
    }
}   

