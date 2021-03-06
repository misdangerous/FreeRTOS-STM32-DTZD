
/**
  ******************************************************************************
  * @file    app_at24cxx.c
  * @author  wangjiaqi
  * @version V0.1
  * @date    2017/3/26
  * @brief   at24cxx系列的驱动文件
  ******************************************************************************
  * @attention
	*地址选取的时候要充分考虑分页问题，16字节一页，超出页会出现数据错误,循环到页头进行循环写
  ******************************************************************************
  */ 
#include "at24cxx.h"
#include "delay.h"


void I2C_InitCfg(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=I2C_SCL|I2C_SDA;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//i2c要求两根上要接上拉电组，如果不接，就要配成Pp模式。
	GPIO_Init(GPIO_I2C,&GPIO_InitStruct);
	I2C_SCL_H;
	I2C_SDA_H;
}

void I2C_SDA_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=I2C_SDA;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_I2C,&GPIO_InitStructure);
}


/*******************************************************************************
**Function Name: 
**Description  :  也可以接收数据
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/
void I2C_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=I2C_SDA;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//配置成上拉，如果板子上没有把这两根线上拉的话。
	GPIO_Init(GPIO_I2C,&GPIO_InitStructure);
}


/*******************************************************************************
**Function Name: 
**Description  :  产生起始信号
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/

void I2C_Start(void)
{
  I2C_SDA_OUT();
	
	I2C_SDA_H;
	I2C_SCL_H;
	delay_us(5);
	I2C_SDA_L;
	delay_us(6);
	I2C_SCL_L;
}


/*******************************************************************************
**Function Name: 
**Description  : 产生停止信号 
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/

void I2C_Stop(void)
{
   I2C_SDA_OUT();

   I2C_SCL_L;
   I2C_SDA_L;
   I2C_SCL_H;
   delay_us(6);
   I2C_SDA_H;
   delay_us(6);
}


/*******************************************************************************
**Function Name: 
**Description  : 主机产生应答信号ACK 
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/

void I2C_Ack(void)
{
   I2C_SCL_L;
   I2C_SDA_OUT();
   I2C_SDA_L;
   delay_us(2);
   I2C_SCL_H;
   delay_us(5);
   I2C_SCL_L;
}


/*******************************************************************************
**Function Name: 
**Description  :  主机不产生应答信号NACK
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/

void I2C_NAck(void)
{
   I2C_SCL_L;
   I2C_SDA_OUT();
   I2C_SDA_H;
   delay_us(2);
   I2C_SCL_H;
   delay_us(5);
   I2C_SCL_L;
}

/*******************************************************************************
**Function Name: 
**Description  :  等待从机应答信号�
**InPut        :NONE
**OutPut       :NONE
**Returns      :1接收应答失败  0 接收应答成功
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/
uint8_t I2C_Wait_Ack(void)
{
	uint8_t tempTime=0;

	I2C_SDA_IN();

	I2C_SDA_H;
	delay_us(1);
	I2C_SCL_H;
	delay_us(1);

	while(GPIO_ReadInputDataBit(GPIO_I2C,I2C_SDA)) //SDA变成低电平的时候，才会给一个应答，如果读到为0说明给反馈信号了。
	{
		tempTime++; //计数器，如果没有应答不能在这里一直等。
		if(tempTime>250)
		{
			I2C_Stop();
			return 1;
		}	 
	}

	I2C_SCL_L;
	return 0;
}

/*******************************************************************************
**Function Name: 
**Description  : I2C 发送一个字节 
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/
void I2C_Send_Byte(uint8_t txd)
{
	uint8_t i=0;

	I2C_SDA_OUT();
	I2C_SCL_L;//拉低时钟开始数据传输

	for(i=0;i<8;i++)
	{
		if((txd&0x80)>0) //0x80  1000 0000 即发送数据时为1就执I2C_SDA_H。否则发个低电平。高位开始先发送
			I2C_SDA_H;
		else
			I2C_SDA_L;

		txd<<=1;
		I2C_SCL_H;//为高电平时才能真正的发送。
		delay_us(2); //发送数据
		I2C_SCL_L;  //准备下一次发送
		delay_us(2);
	}
}


/*******************************************************************************
**Function Name: 
**Description  :I2C 读取一个字节  
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/
uint8_t I2C_Read_Byte(uint8_t ack)
{
   uint8_t i=0,receive=0;

   I2C_SDA_IN();
   for(i=0;i<8;i++)
   {
   		I2C_SCL_L;//从机准备
		delay_us(2);
		I2C_SCL_H;//要开始读了
		receive<<=1;
		if(GPIO_ReadInputDataBit(GPIO_I2C,I2C_SDA)) //高电平时，判断总线状态
		   receive++;//执1操作。
		delay_us(1);	
   }

   	if(ack==0) //说明不用给应答
	   	I2C_NAck();
	else
		I2C_Ack();

	return receive;
}

////////////////////AT24
/*******************************************************************************
**Function Name: 
**Description  :  
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/
uint8_t AT24Cxx_ReadOneByte(uint16_t addr)
{
	uint8_t temp=0;

	I2C_Start();
	
	if(EE_TYPE>AT24C16) //地址就成了双字节寻址了。
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//发送数据地址高位
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//双字节是数据地址低位		
				//单字节是数据地址低位			
	I2C_Wait_Ack();

	I2C_Start();
	I2C_Send_Byte(0xA1);//器件地址，这里只是改为真的读了，即改变下方向，即先写后读
	I2C_Wait_Ack();

	temp=I2C_Read_Byte(0); //  0   代表 NACK
	I2C_Stop();
	
	return temp;	
}

/*******************************************************************************
**Function Name: 
**Description  :  
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/
uint16_t AT24Cxx_ReadTwoByte(uint16_t addr)
{
	uint16_t temp=0;

	I2C_Start();
	
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//发送数据地址高位
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//双字节是数据地址低位		
							//单字节是数据地址低位
	I2C_Wait_Ack();

	I2C_Start();
	I2C_Send_Byte(0xA1+((addr/256)<<1));
	I2C_Wait_Ack();

	temp=I2C_Read_Byte(1); //  1   代表 ACK
	temp<<=8;
	temp|=I2C_Read_Byte(0); //  0  代表 NACK，即读两个字后就发NACK,还可以读3，4个字节等。

	I2C_Stop();
	
	return temp;	
}


/*******************************************************************************
**Function Name: 
**Description  :  
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/
void AT24Cxx_WriteOneByte(uint16_t addr,uint8_t dt)
{
	I2C_Start();

	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//发送数据地址高位
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//双字节是数据地址低位		
				//单字节是数据地址低位
	I2C_Wait_Ack();

	I2C_Send_Byte(dt);//发送娄据
	I2C_Wait_Ack();
	I2C_Stop();

	delay_xms(10);
}


/*******************************************************************************
**Function Name: 
**Description  :  
**InPut        :NONE
**OutPut       :NONE
**Returns      :NONE   
**Author       :CY
**Date         :2016/6/6   
**RevisionHis  :
No.     Version         Date         name    Item    Description              
*1.     V1.0         2016/06/06 
*******************************************************************************/
void AT24Cxx_WriteTwoByte(uint16_t addr,uint16_t dt)
{
	I2C_Start();

	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//发送数据地址高位
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//双字节是数据地址低位		
				//单字节是数据地址低位
	I2C_Wait_Ack();

	I2C_Send_Byte(dt>>8);
	I2C_Wait_Ack();

	I2C_Send_Byte(dt&0xFF);//发低8位这字节数据，发完后就停止。
	I2C_Wait_Ack();

	I2C_Stop();

	delay_xms(10);
}
/*
	函数名称：void AT24Cxx_WriteSerialByte(uint32_t addr,uint8_t *pbuffer,uint16_t len)
	函数输入：addr地址，pbuffer写数据的存储开始地址，len写入数据的长度
	函数功能：连续写入AT24CXX函数
*/

void AT24Cxx_WriteSerialByte(uint32_t addr,uint8_t *pbuffer,uint16_t len)
{
	uint16_t i,j;
	for(j=0;j<((len-1)/8+1);j++)    //八个字节进行一次地址重新写入
	{
		I2C_Start();
		if(EE_TYPE<AT24C16)
		{
			I2C_Send_Byte(0xA0);
			I2C_Wait_Ack();
			I2C_Send_Byte((addr+j*8)>>8);	//发送数据地址高位
		}
		else
		{
			if(EE_TYPE == AT24C256)
			{
				I2C_Send_Byte(0xA0+(((addr+j*8)>>15)<<1));
				I2C_Wait_Ack();
				I2C_Send_Byte((addr+j*8)>>8);
			}
			else
			{
				I2C_Send_Byte(0xA0+(((addr+j*8)/256)<<1));//器件地址+数据地址
			}
		}

		I2C_Wait_Ack();
		I2C_Send_Byte((addr+j*8)%256);//双字节是数据地址低位		
					//单字节是数据地址低位
		I2C_Wait_Ack();
		if(j < (len/8))   //判断剩余字节是否够8个字节
		{
			for(i=0;i<8;i++)
			{
				I2C_Send_Byte(pbuffer[j*8+i]);
				I2C_Wait_Ack();
			}
			I2C_Stop();
		}
		else
		{
			for(i=0;i<len%8;i++)  //不够8个字节或剩余8个字节的处理
			{
				I2C_Send_Byte(pbuffer[j*8+i]);
				I2C_Wait_Ack();
			}
			I2C_Stop();
		}
		delay_xms(10);
	}
}
/*
	函数名称：void AT24Cxx_ReadSerialByte(uint16_t addr,uint8_t *pbuffer,uint16_t len)
	函数输入：addr读取的首地址，pbuffer存储的首地址，len读取数据长度
	函数功能：从addr开始读取len长度个字节
	函数备注：AT24CXX在写入过程中，一次性写入长度有要求，读取没有要求
*/

void AT24Cxx_ReadSerialByte(uint16_t addr,uint8_t *pbuffer,uint16_t len)
{
	uint16_t i;

	I2C_Start();
	
	if(EE_TYPE<AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//发送数据地址高位
	}
	else
	{
		if(EE_TYPE == AT24C256)
		{
			I2C_Send_Byte(0xA0+((addr>>15)<<1));
			I2C_Wait_Ack();
			I2C_Send_Byte(addr>>8);
		}
		else
		{
			I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址
		}
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//双字节是数据地址低位		
							//单字节是数据地址低位
	I2C_Wait_Ack();

	I2C_Start();
	if(EE_TYPE == AT24C256)
	{
		I2C_Send_Byte(0xA1+((addr>>15)<<1));
	}
	else
	{
		I2C_Send_Byte(0xA1+((addr/256)<<1));
	}
	I2C_Wait_Ack();

	for(i=0;i<len-1;i++)
	{
		pbuffer[i]=I2C_Read_Byte(1); //  1   代表 ACK
	}
	pbuffer[i]=I2C_Read_Byte(0); //  0  代表 NACK，即读两个字后就发NACK,还可以读3，4个字节等。

	I2C_Stop();
	
}
