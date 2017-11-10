#include "FreeRTOS.h"
#include "sense_485.h"
#include "sense_modbus.h"
#include "semphr.h"
#include "queue.h"
#include <string.h>

struct Sense Sense_Modbus;


//二值信号量句柄
SemaphoreHandle_t Sense_TxBinarySemaphore;	//二值信号量句柄
SemaphoreHandle_t Sense_RxBinarySemaphore;	//二值信号量句柄

void Sense_485_Init(u32 bound)
{
	DMA_InitCfg();
	USART3_InitCfg(bound);
	Sense_TxBinarySemaphore=xSemaphoreCreateBinary();
	Sense_RxBinarySemaphore=xSemaphoreCreateBinary();
}


//DMA配置

 void DMA_InitCfg(void)
{

	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	//串口发送DMA连接初始化
	DMA_DeInit(DMA1_Channel2);																	//具体哪个通道，可以参考图片教程。
	DMA_InitStructure.DMA_PeripheralBaseAddr 		= 	(uint32_t)&(USART3->DR);  				//DMA在外设与存储器，或存储器与存储器之间搬。两个DMA，第一个有7个通道，第二个有5个通道。
//	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(u32)&ADCConvertedValue;
	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(uint32_t)Sense_Modbus.Send.Buff;
	DMA_InitStructure.DMA_DIR 						= 	DMA_DIR_PeripheralDST;					//外设作为数据传输来源。DMA_DIR_PeripheralDST作目的地。
	DMA_InitStructure.DMA_BufferSize 				= 	BUFFLEN;//N*M sizeof(ADC_ConvertedValue);   	//DEV_MAX_AI_CHNLS;//数据传输的数据目。用以定义指定DMA通道的DMA缓存的大小，单位为数据单位，根据传方向，数据单位等结构中参数DMA_PeripheralDataSize或者DMA_MemoryDataSize，缓存大小一般就是上面定义数组大小ADC_ConvertedValue。
	DMA_InitStructure.DMA_PeripheralInc 			= 	DMA_PeripheralInc_Disable;				//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc 				= 	DMA_MemoryInc_Enable;					//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize 		= 	DMA_PeripheralDataSize_Byte;		//因为寄存器是16位的。
	DMA_InitStructure.DMA_MemoryDataSize 			= 	DMA_MemoryDataSize_Byte;			//独立数据源与目标数据区的传输宽度（字节，半字，全字），8 位，16位，32位
//	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Circular;						//DMA_Mode_Normal(只传送一次),DMA_Mode_Circular (不停的传送)
	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Normal;						//DMA_Mode_Normal(只传送一次),DMA_Mode_Circular (不停的传送)
	DMA_InitStructure.DMA_Priority 					= 	DMA_Priority_High;						//DMA仲载器，即优先级。很高，高，中 低四个级别。级别相同时，响应有硬件来绝定，请求0优先请求1.
	DMA_InitStructure.DMA_M2M 						= 	DMA_M2M_Disable;						//使能DMA通道的内存到内存传输。DMA通道X没有设置为内存到内存传输。
	DMA_Init(DMA1_Channel2, &DMA_InitStructure); 												//第一步 初始化
	
	//DMA_ITConfig(DMA1_Channel2,DMA_IT_TC |DMA_IT_TE,ENABLE);  									//DMA_IT_TC传输完成后中断。DMA_IT_TE传输错误中断， DMA_IT_HT传输过半中断
	DMA_Cmd(DMA1_Channel2, DISABLE);	
	
	
		//串口接收DMA连接初始化
	DMA_DeInit(DMA1_Channel3);																	//具体哪个通道，可以参考图片教程。
	DMA_InitStructure.DMA_PeripheralBaseAddr 		= 	(uint32_t)&(USART3->DR);  				//DMA在外设与存储器，或存储器与存储器之间搬。两个DMA，第一个有7个通道，第二个有5个通道。
//	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(u32)&ADCConvertedValue;
	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(uint32_t)Sense_Modbus.Rece.Buff;
	DMA_InitStructure.DMA_DIR 						= 	DMA_DIR_PeripheralSRC;					//外设作为数据传输来源。DMA_DIR_PeripheralDST作目的地。
	DMA_InitStructure.DMA_BufferSize 				= 	BUFFLEN;//sizeof(ADC_ConvertedValue);   	//DEV_MAX_AI_CHNLS;//数据传输的数据目。用以定义指定DMA通道的DMA缓存的大小，单位为数据单位，根据传方向，数据单位等结构中参数DMA_PeripheralDataSize或者DMA_MemoryDataSize，缓存大小一般就是上面定义数组大小ADC_ConvertedValue。
	DMA_InitStructure.DMA_PeripheralInc 			= 	DMA_PeripheralInc_Disable;				//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc 				= 	DMA_MemoryInc_Enable;					//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize 		= 	DMA_PeripheralDataSize_Byte;		//因为寄存器是16位的。
	DMA_InitStructure.DMA_MemoryDataSize 			= 	DMA_MemoryDataSize_Byte;			//独立数据源与目标数据区的传输宽度（字节，半字，全字），8 位，16位，32位
//	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Circular;						//DMA_Mode_Normal(只传送一次),DMA_Mode_Circular (不停的传送)
	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Normal;						//DMA_Mode_Normal(只传送一次),DMA_Mode_Circular (不停的传送)
	DMA_InitStructure.DMA_Priority 					= 	DMA_Priority_High;						//DMA仲载器，即优先级。很高，高，中 低四个级别。级别相同时，响应有硬件来绝定，请求0优先请求1.
	DMA_InitStructure.DMA_M2M 						= 	DMA_M2M_Disable;						//使能DMA通道的内存到内存传输。DMA通道X没有设置为内存到内存传输。
	DMA_Init(DMA1_Channel3, &DMA_InitStructure); 												//第一步 初始化
	
	//DMA_ITConfig(DMA1_Channel5,DMA_IT_TC |DMA_IT_TE,ENABLE);  									//DMA_IT_TC传输完成后中断。DMA_IT_TE传输错误中断， DMA_IT_HT传输过半中断
	DMA_Cmd(DMA1_Channel3, ENABLE);
}


void USART3_InitCfg(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART2，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	//USART3_TX   GPIOB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.2
   
	//USART3_RX	  GPIOB.11初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB.11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.3  
	
	//USART3_EN	  GPIOB.15初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PB.5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB.5
	
	//Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;//抢占优先级7
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	USART_ClearITPendingBit(USART3,USART_IT_TC);
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USART3,USART_IT_TC,ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);  // 开启串口DMA接收
	USART_Cmd(USART3, ENABLE);                    //使能串口3
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
	DMA_SetCurrDataCounter(DMA1_Channel2,len);	//DMA通道的DMA缓存的大小
	DMA_Cmd(DMA1_Channel2, ENABLE);
}
/*
	函数名称：void UsartRX_DMAReConfig(uint8_t len)
	函数功能：重新设置RX接收的DMA长度
*/
void UsartRX_DMAReConfig(uint8_t len)
{
	DMA_Cmd(DMA1_Channel3, DISABLE);	
	DMA_SetCurrDataCounter(DMA1_Channel3,len);	//DMA通道的DMA缓存的大小
	DMA_Cmd(DMA1_Channel3, ENABLE);
}

/*
串口发送函数:UartSend(uint8_t *buf,uint8_t len)
*buf:数组地址	len：数据长度
返回值：无
*/
void UartSend(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	BaseType_t err=pdFALSE;
	err=xSemaphoreTake(Sense_TxBinarySemaphore,portMAX_DELAY);	//获取信号量
	if(err==pdTRUE)										//获取信号量成功
	{
		Sense_Modbus.Send.DataLen = len;
		for(i=0;i<len;i++)
		{
			Sense_Modbus.Send.Buff[i]=*(buf++);
		}
		Sense_485_ModeTx();                     //设置为发射模式
		USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); 	//使能串口1的DMA发送 
		UsartTX_DMAReConfig(len);
	}
}
/*
*函数：uint8_t UartRece(uint8_t *pbuffer,uint8_t *len)
*功能：将接收到的数据传送到指针pbuffer中，len为长度
*返回值：如果传送成功，返回1，失败返回0
*/

uint8_t UartRece(uint8_t *pbuffer,uint8_t *len)
{
	uint8_t i;
	BaseType_t err=pdFALSE;
	
	err=xSemaphoreTake(Sense_RxBinarySemaphore,1000);	//获取信号量
	if(err==pdPASS)			//接收到消息
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
	函数名称：void USART1_Interrupt(void)
	函数功能：中断调用，如果出现总线空闲，产生中断
*/
void USART3_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)	 
	{
		uint16_t num=0;
		num = USART3->SR;
		num = USART3->DR; 										//清USART_IT_IDLE标志
		num = BUFFLEN - DMA_GetCurrDataCounter(DMA1_Channel3);	//计算接收到多少字节数据
		UsartRX_DMAReConfig(BUFFLEN);
		Sense_Modbus.Rece.DataLen = num;  		//一帧接收完时把一帧共接收到多少字节记录下来。
		if(num > 3)
		{
			xSemaphoreGiveFromISR(Sense_RxBinarySemaphore,&xHigherPriorityTaskWoken);	//释放二值信号量
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
		}
	}
	if((USART_GetITStatus(USART3, USART_IT_TC) != RESET)&&(Sense_TxBinarySemaphore!=NULL))
	{
		USART_ClearITPendingBit(USART3,USART_IT_TC);
		Sense_485_ModeRx();
		xSemaphoreGiveFromISR(Sense_TxBinarySemaphore,&xHigherPriorityTaskWoken);	//释放二值信号量
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
	}
}

