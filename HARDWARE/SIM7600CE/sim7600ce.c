
#include "sim7600ce.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include <string.h>

struct SIM7600CE SIM7600CE_Modbus;

//二值信号量句柄
SemaphoreHandle_t SIM7600CE_TxBinarySemaphore;	//二值信号量句柄
//二值信号量句柄
SemaphoreHandle_t SIM7600CE_RxBinarySemaphore;	//二值信号量句柄
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

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //PWRKEY-->PA.0 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);						 //PA.0 输出低
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //REST-->PA.1 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);						 //PA.1 输出低
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


//DMA配置

 void USART2_DMA_InitCfg(void)
{

	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	//串口发送DMA连接初始化
	DMA_DeInit(DMA1_Channel7);																	//具体哪个通道，可以参考图片教程。
	DMA_InitStructure.DMA_PeripheralBaseAddr 		= 	(uint32_t)&(USART2->DR);  				//DMA在外设与存储器，或存储器与存储器之间搬。两个DMA，第一个有7个通道，第二个有5个通道。
//	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(u32)&ADCConvertedValue;
	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(uint32_t)SIM7600CE_Modbus.Send.Buff;
	DMA_InitStructure.DMA_DIR 						= 	DMA_DIR_PeripheralDST;					//外设作为数据传输来源。DMA_DIR_PeripheralDST作目的地。
	DMA_InitStructure.DMA_BufferSize 				= 	SIM7600CE_BUFFLEN;//N*M sizeof(ADC_ConvertedValue);   	//DEV_MAX_AI_CHNLS;//数据传输的数据目。用以定义指定DMA通道的DMA缓存的大小，单位为数据单位，根据传方向，数据单位等结构中参数DMA_PeripheralDataSize或者DMA_MemoryDataSize，缓存大小一般就是上面定义数组大小ADC_ConvertedValue。
	DMA_InitStructure.DMA_PeripheralInc 			= 	DMA_PeripheralInc_Disable;				//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc 				= 	DMA_MemoryInc_Enable;					//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize 		= 	DMA_PeripheralDataSize_Byte;		//因为寄存器是16位的。
	DMA_InitStructure.DMA_MemoryDataSize 			= 	DMA_MemoryDataSize_Byte;			//独立数据源与目标数据区的传输宽度（字节，半字，全字），8 位，16位，32位
//	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Circular;						//DMA_Mode_Normal(只传送一次),DMA_Mode_Circular (不停的传送)
	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Normal;						//DMA_Mode_Normal(只传送一次),DMA_Mode_Circular (不停的传送)
	DMA_InitStructure.DMA_Priority 					= 	DMA_Priority_High;						//DMA仲载器，即优先级。很高，高，中 低四个级别。级别相同时，响应有硬件来绝定，请求0优先请求1.
	DMA_InitStructure.DMA_M2M 						= 	DMA_M2M_Disable;						//使能DMA通道的内存到内存传输。DMA通道X没有设置为内存到内存传输。
	DMA_Init(DMA1_Channel7, &DMA_InitStructure); 												//第一步 初始化
	
	//DMA_ITConfig(DMA1_Channel7,DMA_IT_TC |DMA_IT_TE,ENABLE);  									//DMA_IT_TC传输完成后中断。DMA_IT_TE传输错误中断， DMA_IT_HT传输过半中断
	DMA_Cmd(DMA1_Channel7, DISABLE);	
	
	
		//串口接收DMA连接初始化
	DMA_DeInit(DMA1_Channel6);																	//具体哪个通道，可以参考图片教程。
	DMA_InitStructure.DMA_PeripheralBaseAddr 		= 	(uint32_t)&(USART2->DR);  				//DMA在外设与存储器，或存储器与存储器之间搬。两个DMA，第一个有7个通道，第二个有5个通道。
//	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(u32)&ADCConvertedValue;
	DMA_InitStructure.DMA_MemoryBaseAddr 			= 	(uint32_t)SIM7600CE_Modbus.Rece.Buff;
	DMA_InitStructure.DMA_DIR 						= 	DMA_DIR_PeripheralSRC;					//外设作为数据传输来源。DMA_DIR_PeripheralDST作目的地。
	DMA_InitStructure.DMA_BufferSize 				= 	SIM7600CE_BUFFLEN;//sizeof(ADC_ConvertedValue);   	//DEV_MAX_AI_CHNLS;//数据传输的数据目。用以定义指定DMA通道的DMA缓存的大小，单位为数据单位，根据传方向，数据单位等结构中参数DMA_PeripheralDataSize或者DMA_MemoryDataSize，缓存大小一般就是上面定义数组大小ADC_ConvertedValue。
	DMA_InitStructure.DMA_PeripheralInc 			= 	DMA_PeripheralInc_Disable;				//外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc 				= 	DMA_MemoryInc_Enable;					//内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize 		= 	DMA_PeripheralDataSize_Byte;		//因为寄存器是16位的。
	DMA_InitStructure.DMA_MemoryDataSize 			= 	DMA_MemoryDataSize_Byte;			//独立数据源与目标数据区的传输宽度（字节，半字，全字），8 位，16位，32位
//	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Circular;						//DMA_Mode_Normal(只传送一次),DMA_Mode_Circular (不停的传送)
	DMA_InitStructure.DMA_Mode 						= 	DMA_Mode_Normal;						//DMA_Mode_Normal(只传送一次),DMA_Mode_Circular (不停的传送)
	DMA_InitStructure.DMA_Priority 					= 	DMA_Priority_High;						//DMA仲载器，即优先级。很高，高，中 低四个级别。级别相同时，响应有硬件来绝定，请求0优先请求1.
	DMA_InitStructure.DMA_M2M 						= 	DMA_M2M_Disable;						//使能DMA通道的内存到内存传输。DMA通道X没有设置为内存到内存传输。
	DMA_Init(DMA1_Channel6, &DMA_InitStructure); 												//第一步 初始化
	
	//DMA_ITConfig(DMA1_Channel6,DMA_IT_TC |DMA_IT_TE,ENABLE);  									//DMA_IT_TC传输完成后中断。DMA_IT_TE传输错误中断， DMA_IT_HT传输过半中断
	DMA_Cmd(DMA1_Channel6, ENABLE);
}


void USART2_InitCfg(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART2，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
	//USART2_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  
	
	
	//USART2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
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

	USART_Init(USART2, &USART_InitStructure); //初始化串口3
	USART_ClearITPendingBit(USART2,USART_IT_TC);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
	USART_ITConfig(USART2,USART_IT_TC,ENABLE);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);  // 开启串口DMA接收
	USART_Cmd(USART2, ENABLE);                    //使能串口3
}



void Usart2TX_DMAReConfig(uint8_t len)
{
	DMA_Cmd(DMA1_Channel7, DISABLE);	
	DMA_SetCurrDataCounter(DMA1_Channel7,len);	//DMA通道的DMA缓存的大小
	DMA_Cmd(DMA1_Channel7, ENABLE);
}
/*
	函数名称：void UsartRX_DMAReConfig(uint8_t len)
	函数功能：重新设置RX接收的DMA长度
*/
void Usart2RX_DMAReConfig(uint16_t len)
{
	DMA_Cmd(DMA1_Channel6, DISABLE);	
	DMA_SetCurrDataCounter(DMA1_Channel6,len);	//DMA通道的DMA缓存的大小
	DMA_Cmd(DMA1_Channel6, ENABLE);
}

/*
串口发送函数:UartSend(uint8_t *buf,uint8_t len)
*buf:数组地址	len：数据长度
返回值：无
*/
void SIM7600CESend(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	BaseType_t err=pdFALSE;
	err=xSemaphoreTake(SIM7600CE_TxBinarySemaphore,portMAX_DELAY);	//获取信号量
	if(err==pdTRUE)										//获取信号量成功
	{
		SIM7600CE_Modbus.Send.DataLen = len;
		for(i=0;i<len;i++)
		{
			SIM7600CE_Modbus.Send.Buff[i]=*(buf++);
		}
		USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE); 	//使能串口1的DMA发送 
		Usart2TX_DMAReConfig(len);
	}
}
/*
*函数：uint8_t UartRece(uint8_t *pbuffer,uint8_t *len)
*功能：将接收到的数据传送到指针pbuffer中，len为长度
*返回值：如果传送成功，返回1，失败返回0
*/

uint8_t SIM7600CERece(uint8_t *pbuffer,uint8_t *len,TickType_t pportMAX_DELAY)
{
	uint8_t i;
	BaseType_t err=pdFALSE;
	
	err=xSemaphoreTake(SIM7600CE_RxBinarySemaphore,portMAX_DELAY);	//获取信号量
	if(err==pdTRUE)			//接收到消息
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
	函数名称：void USART1_Interrupt(void)
	函数功能：中断调用，如果出现总线空闲，产生中断
*/
void USART2_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)	 
	{
		uint16_t num=0;
		num = USART2->SR;
		num = USART2->DR; 										//清USART_IT_IDLE标志
		SIM7600CE_Modbus.Rece.DataLen = SIM7600CE_BUFFLEN - DMA_GetCurrDataCounter(DMA1_Channel6);	//计算接收到多少字节数据
		Usart2RX_DMAReConfig(SIM7600CE_BUFFLEN);
		xSemaphoreGiveFromISR(SIM7600CE_RxBinarySemaphore,&xHigherPriorityTaskWoken);	//释放二值信号量
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
	}
	if((USART_GetITStatus(USART2, USART_IT_TC) != RESET)&&(SIM7600CE_TxBinarySemaphore!=NULL))
	{
		USART_ClearITPendingBit(USART2,USART_IT_TC);
		xSemaphoreGiveFromISR(SIM7600CE_TxBinarySemaphore,&xHigherPriorityTaskWoken);	//释放二值信号量
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
	}
}


