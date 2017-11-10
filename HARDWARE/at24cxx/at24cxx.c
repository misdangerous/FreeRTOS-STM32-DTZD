
/**
  ******************************************************************************
  * @file    app_at24cxx.c
  * @author  wangjiaqi
  * @version V0.1
  * @date    2017/3/26
  * @brief   at24cxxϵ�е������ļ�
  ******************************************************************************
  * @attention
	*��ַѡȡ��ʱ��Ҫ��ֿ��Ƿ�ҳ���⣬16�ֽ�һҳ������ҳ��������ݴ���,ѭ����ҳͷ����ѭ��д
  ******************************************************************************
  */ 
#include "at24cxx.h"
#include "delay.h"


void I2C_InitCfg(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=I2C_SCL|I2C_SDA;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//i2cҪ��������Ҫ���������飬������ӣ���Ҫ���Ppģʽ��
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
**Description  :  Ҳ���Խ�������
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
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//���ó����������������û�а��������������Ļ���
	GPIO_Init(GPIO_I2C,&GPIO_InitStructure);
}


/*******************************************************************************
**Function Name: 
**Description  :  ������ʼ�ź�
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
**Description  : ����ֹͣ�ź� 
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
**Description  : ��������Ӧ���ź�ACK 
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
**Description  :  ����������Ӧ���ź�NACK
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
**Description  :  �ȴ��ӻ�Ӧ���źź
**InPut        :NONE
**OutPut       :NONE
**Returns      :1����Ӧ��ʧ��  0 ����Ӧ��ɹ�
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

	while(GPIO_ReadInputDataBit(GPIO_I2C,I2C_SDA)) //SDA��ɵ͵�ƽ��ʱ�򣬲Ż��һ��Ӧ���������Ϊ0˵���������ź��ˡ�
	{
		tempTime++; //�����������û��Ӧ����������һֱ�ȡ�
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
**Description  : I2C ����һ���ֽ� 
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
	I2C_SCL_L;//����ʱ�ӿ�ʼ���ݴ���

	for(i=0;i<8;i++)
	{
		if((txd&0x80)>0) //0x80  1000 0000 ����������ʱΪ1��ִI2C_SDA_H�����򷢸��͵�ƽ����λ��ʼ�ȷ���
			I2C_SDA_H;
		else
			I2C_SDA_L;

		txd<<=1;
		I2C_SCL_H;//Ϊ�ߵ�ƽʱ���������ķ��͡�
		delay_us(2); //��������
		I2C_SCL_L;  //׼����һ�η���
		delay_us(2);
	}
}


/*******************************************************************************
**Function Name: 
**Description  :I2C ��ȡһ���ֽ�  
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
   		I2C_SCL_L;//�ӻ�׼��
		delay_us(2);
		I2C_SCL_H;//Ҫ��ʼ����
		receive<<=1;
		if(GPIO_ReadInputDataBit(GPIO_I2C,I2C_SDA)) //�ߵ�ƽʱ���ж�����״̬
		   receive++;//ִ1������
		delay_us(1);	
   }

   	if(ack==0) //˵�����ø�Ӧ��
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
	
	if(EE_TYPE>AT24C16) //��ַ�ͳ���˫�ֽ�Ѱַ�ˡ�
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
				//���ֽ������ݵ�ַ��λ			
	I2C_Wait_Ack();

	I2C_Start();
	I2C_Send_Byte(0xA1);//������ַ������ֻ�Ǹ�Ϊ��Ķ��ˣ����ı��·��򣬼���д���
	I2C_Wait_Ack();

	temp=I2C_Read_Byte(0); //  0   ���� NACK
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
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
							//���ֽ������ݵ�ַ��λ
	I2C_Wait_Ack();

	I2C_Start();
	I2C_Send_Byte(0xA1+((addr/256)<<1));
	I2C_Wait_Ack();

	temp=I2C_Read_Byte(1); //  1   ���� ACK
	temp<<=8;
	temp|=I2C_Read_Byte(0); //  0  ���� NACK�����������ֺ�ͷ�NACK,�����Զ�3��4���ֽڵȡ�

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
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
				//���ֽ������ݵ�ַ��λ
	I2C_Wait_Ack();

	I2C_Send_Byte(dt);//����¦��
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
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
				//���ֽ������ݵ�ַ��λ
	I2C_Wait_Ack();

	I2C_Send_Byte(dt>>8);
	I2C_Wait_Ack();

	I2C_Send_Byte(dt&0xFF);//����8λ���ֽ����ݣ�������ֹͣ��
	I2C_Wait_Ack();

	I2C_Stop();

	delay_xms(10);
}
/*
	�������ƣ�void AT24Cxx_WriteSerialByte(uint32_t addr,uint8_t *pbuffer,uint16_t len)
	�������룺addr��ַ��pbufferд���ݵĴ洢��ʼ��ַ��lenд�����ݵĳ���
	�������ܣ�����д��AT24CXX����
*/

void AT24Cxx_WriteSerialByte(uint32_t addr,uint8_t *pbuffer,uint16_t len)
{
	uint16_t i,j;
	for(j=0;j<((len-1)/8+1);j++)    //�˸��ֽڽ���һ�ε�ַ����д��
	{
		I2C_Start();
		if(EE_TYPE<AT24C16)
		{
			I2C_Send_Byte(0xA0);
			I2C_Wait_Ack();
			I2C_Send_Byte((addr+j*8)>>8);	//�������ݵ�ַ��λ
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
				I2C_Send_Byte(0xA0+(((addr+j*8)/256)<<1));//������ַ+���ݵ�ַ
			}
		}

		I2C_Wait_Ack();
		I2C_Send_Byte((addr+j*8)%256);//˫�ֽ������ݵ�ַ��λ		
					//���ֽ������ݵ�ַ��λ
		I2C_Wait_Ack();
		if(j < (len/8))   //�ж�ʣ���ֽ��Ƿ�8���ֽ�
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
			for(i=0;i<len%8;i++)  //����8���ֽڻ�ʣ��8���ֽڵĴ���
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
	�������ƣ�void AT24Cxx_ReadSerialByte(uint16_t addr,uint8_t *pbuffer,uint16_t len)
	�������룺addr��ȡ���׵�ַ��pbuffer�洢���׵�ַ��len��ȡ���ݳ���
	�������ܣ���addr��ʼ��ȡlen���ȸ��ֽ�
	������ע��AT24CXX��д������У�һ����д�볤����Ҫ�󣬶�ȡû��Ҫ��
*/

void AT24Cxx_ReadSerialByte(uint16_t addr,uint8_t *pbuffer,uint16_t len)
{
	uint16_t i;

	I2C_Start();
	
	if(EE_TYPE<AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
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
			I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
		}
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
							//���ֽ������ݵ�ַ��λ
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
		pbuffer[i]=I2C_Read_Byte(1); //  1   ���� ACK
	}
	pbuffer[i]=I2C_Read_Byte(0); //  0  ���� NACK�����������ֺ�ͷ�NACK,�����Զ�3��4���ֽڵȡ�

	I2C_Stop();
	
}
