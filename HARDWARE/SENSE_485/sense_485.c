#include "FreeRTOS.h"
#include "sense_485.h"
#include "sense_modbus.h"
#include "semphr.h"
#include "queue.h"
#include <string.h>

struct Sense Sense_Modbus;


//��ֵ�ź������
SemaphoreHandle_t Sense_TxBinarySemaphore;	//��ֵ�ź������
SemaphoreHandle_t Sense_RxBinarySemaphore;	//��ֵ�ź������

void Sense_485_Init(u32 bound)
{
	DMA_InitCfg();
	USART3_InitCfg(bound);
	Sense_TxBinarySemaphore=xSemaphoreCreateBinary();
	Sense_RxBinarySemaphore=xSemaphoreCreateBinary();
}


//DMA����

 void DMA_InitCfg(void)
{

	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	//���ڷ���DMA���ӳ�ʼ��
	DMA_DeInit(DMA1_Channel2);																	//�����ĸ�ͨ�������Բο�ͼƬ�̡̳�
	DMA_InitStructure.DMA_PeripheralBaseAddr 		= 	(uint32_t)&(USART3->DR);  				//DMA��������洢������洢����洢��֮��ᡣ����DMA����һ����7��ͨ�����ڶ�����5��ͨ����
//	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(u32)&ADCConvertedValue;
	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(uint32_t)Sense_Modbus.Send.Buff;
	DMA_InitStructure.DMA_DIR 						= 	DMA_DIR_PeripheralDST;					//������Ϊ���ݴ�����Դ��DMA_DIR_PeripheralDST��Ŀ�ĵء�
	DMA_InitStructure.DMA_BufferSize 				= 	BUFFLEN;//N*M sizeof(ADC_ConvertedValue);   	//DEV_MAX_AI_CHNLS;//���ݴ��������Ŀ�����Զ���ָ��DMAͨ����DMA����Ĵ�С����λΪ���ݵ�λ�����ݴ��������ݵ�λ�Ƚṹ�в���DMA_PeripheralDataSize����DMA_MemoryDataSize�������Сһ��������涨�������СADC_ConvertedValue��
	DMA_InitStructure.DMA_PeripheralInc 			= 	DMA_PeripheralInc_Disable;				//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc 				= 	DMA_MemoryInc_Enable;					//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize 		= 	DMA_PeripheralDataSize_Byte;		//��Ϊ�Ĵ�����16λ�ġ�
	DMA_InitStructure.DMA_MemoryDataSize 			= 	DMA_MemoryDataSize_Byte;			//��������Դ��Ŀ���������Ĵ����ȣ��ֽڣ����֣�ȫ�֣���8 λ��16λ��32λ
//	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Circular;						//DMA_Mode_Normal(ֻ����һ��),DMA_Mode_Circular (��ͣ�Ĵ���)
	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Normal;						//DMA_Mode_Normal(ֻ����һ��),DMA_Mode_Circular (��ͣ�Ĵ���)
	DMA_InitStructure.DMA_Priority 					= 	DMA_Priority_High;						//DMA�������������ȼ����ܸߣ��ߣ��� ���ĸ����𡣼�����ͬʱ����Ӧ��Ӳ��������������0��������1.
	DMA_InitStructure.DMA_M2M 						= 	DMA_M2M_Disable;						//ʹ��DMAͨ�����ڴ浽�ڴ洫�䡣DMAͨ��Xû������Ϊ�ڴ浽�ڴ洫�䡣
	DMA_Init(DMA1_Channel2, &DMA_InitStructure); 												//��һ�� ��ʼ��
	
	//DMA_ITConfig(DMA1_Channel2,DMA_IT_TC |DMA_IT_TE,ENABLE);  									//DMA_IT_TC������ɺ��жϡ�DMA_IT_TE��������жϣ� DMA_IT_HT��������ж�
	DMA_Cmd(DMA1_Channel2, DISABLE);	
	
	
		//���ڽ���DMA���ӳ�ʼ��
	DMA_DeInit(DMA1_Channel3);																	//�����ĸ�ͨ�������Բο�ͼƬ�̡̳�
	DMA_InitStructure.DMA_PeripheralBaseAddr 		= 	(uint32_t)&(USART3->DR);  				//DMA��������洢������洢����洢��֮��ᡣ����DMA����һ����7��ͨ�����ڶ�����5��ͨ����
//	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(u32)&ADCConvertedValue;
	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(uint32_t)Sense_Modbus.Rece.Buff;
	DMA_InitStructure.DMA_DIR 						= 	DMA_DIR_PeripheralSRC;					//������Ϊ���ݴ�����Դ��DMA_DIR_PeripheralDST��Ŀ�ĵء�
	DMA_InitStructure.DMA_BufferSize 				= 	BUFFLEN;//sizeof(ADC_ConvertedValue);   	//DEV_MAX_AI_CHNLS;//���ݴ��������Ŀ�����Զ���ָ��DMAͨ����DMA����Ĵ�С����λΪ���ݵ�λ�����ݴ��������ݵ�λ�Ƚṹ�в���DMA_PeripheralDataSize����DMA_MemoryDataSize�������Сһ��������涨�������СADC_ConvertedValue��
	DMA_InitStructure.DMA_PeripheralInc 			= 	DMA_PeripheralInc_Disable;				//�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc 				= 	DMA_MemoryInc_Enable;					//�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize 		= 	DMA_PeripheralDataSize_Byte;		//��Ϊ�Ĵ�����16λ�ġ�
	DMA_InitStructure.DMA_MemoryDataSize 			= 	DMA_MemoryDataSize_Byte;			//��������Դ��Ŀ���������Ĵ����ȣ��ֽڣ����֣�ȫ�֣���8 λ��16λ��32λ
//	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Circular;						//DMA_Mode_Normal(ֻ����һ��),DMA_Mode_Circular (��ͣ�Ĵ���)
	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Normal;						//DMA_Mode_Normal(ֻ����һ��),DMA_Mode_Circular (��ͣ�Ĵ���)
	DMA_InitStructure.DMA_Priority 					= 	DMA_Priority_High;						//DMA�������������ȼ����ܸߣ��ߣ��� ���ĸ����𡣼�����ͬʱ����Ӧ��Ӳ��������������0��������1.
	DMA_InitStructure.DMA_M2M 						= 	DMA_M2M_Disable;						//ʹ��DMAͨ�����ڴ浽�ڴ洫�䡣DMAͨ��Xû������Ϊ�ڴ浽�ڴ洫�䡣
	DMA_Init(DMA1_Channel3, &DMA_InitStructure); 												//��һ�� ��ʼ��
	
	//DMA_ITConfig(DMA1_Channel5,DMA_IT_TC |DMA_IT_TE,ENABLE);  									//DMA_IT_TC������ɺ��жϡ�DMA_IT_TE��������жϣ� DMA_IT_HT��������ж�
	DMA_Cmd(DMA1_Channel3, ENABLE);
}


void USART3_InitCfg(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART2��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	//USART3_TX   GPIOB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
	//USART3_RX	  GPIOB.11��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB.11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.3  
	
	//USART3_EN	  GPIOB.15��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PB.5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOB.5
	
	//Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
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

	USART_Init(USART3, &USART_InitStructure); //��ʼ������3
	USART_ClearITPendingBit(USART3,USART_IT_TC);
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USART3,USART_IT_TC,ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);  // ��������DMA����
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3
}


void Sense_485_ModeRx(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}

void Sense_485_ModeTx(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
}

void UsartTX_DMAReConfig(uint8_t len)
{
	DMA_Cmd(DMA1_Channel2, DISABLE);	
	DMA_SetCurrDataCounter(DMA1_Channel2,len);	//DMAͨ����DMA����Ĵ�С
	DMA_Cmd(DMA1_Channel2, ENABLE);
}
/*
	�������ƣ�void UsartRX_DMAReConfig(uint8_t len)
	�������ܣ���������RX���յ�DMA����
*/
void UsartRX_DMAReConfig(uint8_t len)
{
	DMA_Cmd(DMA1_Channel3, DISABLE);	
	DMA_SetCurrDataCounter(DMA1_Channel3,len);	//DMAͨ����DMA����Ĵ�С
	DMA_Cmd(DMA1_Channel3, ENABLE);
}

/*
���ڷ��ͺ���:UartSend(uint8_t *buf,uint8_t len)
*buf:�����ַ	len�����ݳ���
����ֵ����
*/
void UartSend(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	BaseType_t err=pdFALSE;
	err=xSemaphoreTake(Sense_TxBinarySemaphore,portMAX_DELAY);	//��ȡ�ź���
	if(err==pdTRUE)										//��ȡ�ź����ɹ�
	{
		Sense_Modbus.Send.DataLen = len;
		for(i=0;i<len;i++)
		{
			Sense_Modbus.Send.Buff[i]=*(buf++);
		}
		Sense_485_ModeTx();                     //����Ϊ����ģʽ
		USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); 	//ʹ�ܴ���1��DMA���� 
		UsartTX_DMAReConfig(len);
	}
}
/*
*������uint8_t UartRece(uint8_t *pbuffer,uint8_t *len)
*���ܣ������յ������ݴ��͵�ָ��pbuffer�У�lenΪ����
*����ֵ��������ͳɹ�������1��ʧ�ܷ���0
*/

uint8_t UartRece(uint8_t *pbuffer,uint8_t *len)
{
	uint8_t i;
	BaseType_t err=pdFALSE;
	
	err=xSemaphoreTake(Sense_RxBinarySemaphore,1000);	//��ȡ�ź���
	if(err==pdPASS)			//���յ���Ϣ
	{
		*len = Sense_Modbus.Rece.DataLen;
		for(i=0;i<*len;i++)
		{
			pbuffer[i] = Sense_Modbus.Rece.Buff[i];
		}
		return err;
	}
	return err;
}

/*
	�������ƣ�void USART1_Interrupt(void)
	�������ܣ��жϵ��ã�����������߿��У������ж�
*/
void USART3_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)	 
	{
		uint16_t num=0;
		num = USART3->SR;
		num = USART3->DR; 										//��USART_IT_IDLE��־
		num = BUFFLEN - DMA_GetCurrDataCounter(DMA1_Channel3);	//������յ������ֽ�����
		UsartRX_DMAReConfig(BUFFLEN);
		Sense_Modbus.Rece.DataLen = num;  		//һ֡������ʱ��һ֡�����յ������ֽڼ�¼������
		if(num > 3)
		{
			xSemaphoreGiveFromISR(Sense_RxBinarySemaphore,&xHigherPriorityTaskWoken);	//�ͷŶ�ֵ�ź���
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
		}
	}
	if((USART_GetITStatus(USART3, USART_IT_TC) != RESET)&&(Sense_TxBinarySemaphore!=NULL))
	{
		USART_ClearITPendingBit(USART3,USART_IT_TC);
		Sense_485_ModeRx();
		xSemaphoreGiveFromISR(Sense_TxBinarySemaphore,&xHigherPriorityTaskWoken);	//�ͷŶ�ֵ�ź���
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
	}
}

