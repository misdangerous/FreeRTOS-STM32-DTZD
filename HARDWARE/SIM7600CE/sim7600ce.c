
#include "sim7600ce.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include <string.h>

struct SIM7600CE SIM7600CE_Modbus;

//��ֵ�ź������
SemaphoreHandle_t SIM7600CE_TxBinarySemaphore;	//��ֵ�ź������
//��ֵ�ź������
SemaphoreHandle_t SIM7600CE_RxBinarySemaphore;	//��ֵ�ź������
void SIM7600CE_IO_Init(void);

void SIM7600CE_Init(u32 bound)
{
	USART2_DMA_InitCfg();
	USART2_InitCfg(bound);
	SIM7600CE_IO_Init();
	SIM7600CE_TxBinarySemaphore=xSemaphoreCreateBinary();
	SIM7600CE_RxBinarySemaphore=xSemaphoreCreateBinary();
}


void SIM7600CE_IO_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //PWRKEY-->PA.0 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);						 //PA.0 �����
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //REST-->PA.1 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);						 //PA.1 �����
}

void SIM7600CE_PWRKEY(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	vTaskDelay(1000);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
}

void SIM7600CE_RESET(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	vTaskDelay(1000);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}


//DMA����

 void USART2_DMA_InitCfg(void)
{

	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	//���ڷ���DMA���ӳ�ʼ��
	DMA_DeInit(DMA1_Channel7);																	//�����ĸ�ͨ�������Բο�ͼƬ�̡̳�
	DMA_InitStructure.DMA_PeripheralBaseAddr 		= 	(uint32_t)&(USART2->DR);  				//DMA��������洢������洢����洢��֮��ᡣ����DMA����һ����7��ͨ�����ڶ�����5��ͨ����
//	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(u32)&ADCConvertedValue;
	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(uint32_t)SIM7600CE_Modbus.Send.Buff;
	DMA_InitStructure.DMA_DIR 						= 	DMA_DIR_PeripheralDST;					//������Ϊ���ݴ�����Դ��DMA_DIR_PeripheralDST��Ŀ�ĵء�
	DMA_InitStructure.DMA_BufferSize 				= 	SIM7600CE_BUFFLEN;//N*M sizeof(ADC_ConvertedValue);   	//DEV_MAX_AI_CHNLS;//���ݴ��������Ŀ�����Զ���ָ��DMAͨ����DMA����Ĵ�С����λΪ���ݵ�λ�����ݴ��������ݵ�λ�Ƚṹ�в���DMA_PeripheralDataSize����DMA_MemoryDataSize�������Сһ��������涨�������СADC_ConvertedValue��
	DMA_InitStructure.DMA_PeripheralInc 			= 	DMA_PeripheralInc_Disable;				//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc 				= 	DMA_MemoryInc_Enable;					//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize 		= 	DMA_PeripheralDataSize_Byte;		//��Ϊ�Ĵ�����16λ�ġ�
	DMA_InitStructure.DMA_MemoryDataSize 			= 	DMA_MemoryDataSize_Byte;			//��������Դ��Ŀ���������Ĵ����ȣ��ֽڣ����֣�ȫ�֣���8 λ��16λ��32λ
//	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Circular;						//DMA_Mode_Normal(ֻ����һ��),DMA_Mode_Circular (��ͣ�Ĵ���)
	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Normal;						//DMA_Mode_Normal(ֻ����һ��),DMA_Mode_Circular (��ͣ�Ĵ���)
	DMA_InitStructure.DMA_Priority 					= 	DMA_Priority_High;						//DMA�������������ȼ����ܸߣ��ߣ��� ���ĸ����𡣼�����ͬʱ����Ӧ��Ӳ��������������0��������1.
	DMA_InitStructure.DMA_M2M 						= 	DMA_M2M_Disable;						//ʹ��DMAͨ�����ڴ浽�ڴ洫�䡣DMAͨ��Xû������Ϊ�ڴ浽�ڴ洫�䡣
	DMA_Init(DMA1_Channel7, &DMA_InitStructure); 												//��һ�� ��ʼ��
	
	//DMA_ITConfig(DMA1_Channel7,DMA_IT_TC |DMA_IT_TE,ENABLE);  									//DMA_IT_TC������ɺ��жϡ�DMA_IT_TE��������жϣ� DMA_IT_HT��������ж�
	DMA_Cmd(DMA1_Channel7, DISABLE);	
	
	
		//���ڽ���DMA���ӳ�ʼ��
	DMA_DeInit(DMA1_Channel6);																	//�����ĸ�ͨ�������Բο�ͼƬ�̡̳�
	DMA_InitStructure.DMA_PeripheralBaseAddr 		= 	(uint32_t)&(USART2->DR);  				//DMA��������洢������洢����洢��֮��ᡣ����DMA����һ����7��ͨ�����ڶ�����5��ͨ����
//	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(u32)&ADCConvertedValue;
	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(uint32_t)SIM7600CE_Modbus.Rece.Buff;
	DMA_InitStructure.DMA_DIR 						= 	DMA_DIR_PeripheralSRC;					//������Ϊ���ݴ�����Դ��DMA_DIR_PeripheralDST��Ŀ�ĵء�
	DMA_InitStructure.DMA_BufferSize 				= 	SIM7600CE_BUFFLEN;//sizeof(ADC_ConvertedValue);   	//DEV_MAX_AI_CHNLS;//���ݴ��������Ŀ�����Զ���ָ��DMAͨ����DMA����Ĵ�С����λΪ���ݵ�λ�����ݴ��������ݵ�λ�Ƚṹ�в���DMA_PeripheralDataSize����DMA_MemoryDataSize�������Сһ��������涨�������СADC_ConvertedValue��
	DMA_InitStructure.DMA_PeripheralInc 			= 	DMA_PeripheralInc_Disable;				//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc 				= 	DMA_MemoryInc_Enable;					//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize 		= 	DMA_PeripheralDataSize_Byte;		//��Ϊ�Ĵ�����16λ�ġ�
	DMA_InitStructure.DMA_MemoryDataSize 			= 	DMA_MemoryDataSize_Byte;			//��������Դ��Ŀ���������Ĵ����ȣ��ֽڣ����֣�ȫ�֣���8 λ��16λ��32λ
//	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Circular;						//DMA_Mode_Normal(ֻ����һ��),DMA_Mode_Circular (��ͣ�Ĵ���)
	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Normal;						//DMA_Mode_Normal(ֻ����һ��),DMA_Mode_Circular (��ͣ�Ĵ���)
	DMA_InitStructure.DMA_Priority 					= 	DMA_Priority_High;						//DMA�������������ȼ����ܸߣ��ߣ��� ���ĸ����𡣼�����ͬʱ����Ӧ��Ӳ��������������0��������1.
	DMA_InitStructure.DMA_M2M 						= 	DMA_M2M_Disable;						//ʹ��DMAͨ�����ڴ浽�ڴ洫�䡣DMAͨ��Xû������Ϊ�ڴ浽�ڴ洫�䡣
	DMA_Init(DMA1_Channel6, &DMA_InitStructure); 												//��һ�� ��ʼ��
	
	//DMA_ITConfig(DMA1_Channel6,DMA_IT_TC |DMA_IT_TE,ENABLE);  									//DMA_IT_TC������ɺ��жϡ�DMA_IT_TE��������жϣ� DMA_IT_HT��������ж�
	DMA_Cmd(DMA1_Channel6, ENABLE);
}


void USART2_InitCfg(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART2��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
	//USART2_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  
	
	
	//USART2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;//��ռ���ȼ�7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������3
	USART_ClearITPendingBit(USART2,USART_IT_TC);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USART2,USART_IT_TC,ENABLE);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);  // ��������DMA����
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���3
}



void Usart2TX_DMAReConfig(uint8_t len)
{
	DMA_Cmd(DMA1_Channel7, DISABLE);	
	DMA_SetCurrDataCounter(DMA1_Channel7,len);	//DMAͨ����DMA����Ĵ�С
	DMA_Cmd(DMA1_Channel7, ENABLE);
}
/*
	�������ƣ�void UsartRX_DMAReConfig(uint8_t len)
	�������ܣ���������RX���յ�DMA����
*/
void Usart2RX_DMAReConfig(uint16_t len)
{
	DMA_Cmd(DMA1_Channel6, DISABLE);	
	DMA_SetCurrDataCounter(DMA1_Channel6,len);	//DMAͨ����DMA����Ĵ�С
	DMA_Cmd(DMA1_Channel6, ENABLE);
}

/*
���ڷ��ͺ���:UartSend(uint8_t *buf,uint8_t len)
*buf:�����ַ	len�����ݳ���
����ֵ����
*/
void SIM7600CESend(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	BaseType_t err=pdFALSE;
	err=xSemaphoreTake(SIM7600CE_TxBinarySemaphore,portMAX_DELAY);	//��ȡ�ź���
	if(err==pdTRUE)										//��ȡ�ź����ɹ�
	{
		SIM7600CE_Modbus.Send.DataLen = len;
		for(i=0;i<len;i++)
		{
			SIM7600CE_Modbus.Send.Buff[i]=*(buf++);
		}
		USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE); 	//ʹ�ܴ���1��DMA���� 
		Usart2TX_DMAReConfig(len);
	}
}
/*
*������uint8_t UartRece(uint8_t *pbuffer,uint8_t *len)
*���ܣ������յ������ݴ��͵�ָ��pbuffer�У�lenΪ����
*����ֵ��������ͳɹ�������1��ʧ�ܷ���0
*/

uint8_t SIM7600CERece(uint8_t *pbuffer,uint8_t *len,TickType_t pportMAX_DELAY)
{
	uint8_t i;
	BaseType_t err=pdFALSE;
	
	err=xSemaphoreTake(SIM7600CE_RxBinarySemaphore,portMAX_DELAY);	//��ȡ�ź���
	if(err==pdTRUE)			//���յ���Ϣ
	{
		*len = SIM7600CE_Modbus.Rece.DataLen;
//		*len = strlen((char *)SIM7600CE_Modbus.Rece.Buff);
//		SIM7600CE_Modbus.Rece.DataLen = strlen((char *)SIM7600CE_Modbus.Rece.Buff);
		for(i=0;i<*len;i++)
		{
			pbuffer[i] = SIM7600CE_Modbus.Rece.Buff[i];
		}
		memset(SIM7600CE_Modbus.Rece.Buff,0,SIM7600CE_BUFFLEN);
		return 1;
	}
	return err;
}

/*
	�������ƣ�void USART1_Interrupt(void)
	�������ܣ��жϵ��ã�����������߿��У������ж�
*/
void USART2_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)	 
	{
		uint16_t num=0;
		num = USART2->SR;
		num = USART2->DR; 										//��USART_IT_IDLE��־
		SIM7600CE_Modbus.Rece.DataLen = SIM7600CE_BUFFLEN - DMA_GetCurrDataCounter(DMA1_Channel6);	//������յ������ֽ�����
		Usart2RX_DMAReConfig(SIM7600CE_BUFFLEN);
		xSemaphoreGiveFromISR(SIM7600CE_RxBinarySemaphore,&xHigherPriorityTaskWoken);	//�ͷŶ�ֵ�ź���
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
	}
	if((USART_GetITStatus(USART2, USART_IT_TC) != RESET)&&(SIM7600CE_TxBinarySemaphore!=NULL))
	{
		USART_ClearITPendingBit(USART2,USART_IT_TC);
		xSemaphoreGiveFromISR(SIM7600CE_TxBinarySemaphore,&xHigherPriorityTaskWoken);	//�ͷŶ�ֵ�ź���
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
	}
}


