
/**
  ******************************************************************************
  * @file    app_at24cxx.c
  * @author  wangjiaqi
  * @version V0.1
  * @date    2017/3/26
  * @brief   at24cxxÏµÁĞµÄÇı¶¯ÎÄ¼ş
  ******************************************************************************
  * @attention
	*µØÖ·Ñ¡È¡µÄÊ±ºòÒª³ä·Ö¿¼ÂÇ·ÖÒ³ÎÊÌâ£¬16×Ö½ÚÒ»Ò³£¬³¬³öÒ³»á³öÏÖÊı¾İ´íÎó,Ñ­»·µ½Ò³Í·½øĞĞÑ­»·Ğ´
  ******************************************************************************
  */ 
#include "at24cxx.h"
#include "delay.h"


void I2C_InitCfg(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=I2C_SCL|I2C_SDA;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//i2cÒªÇóÁ½¸ùÉÏÒª½ÓÉÏÀ­µç×é£¬Èç¹û²»½Ó£¬¾ÍÒªÅä³ÉPpÄ£Ê½¡£
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
**Description  :  Ò²¿ÉÒÔ½ÓÊÕÊı¾İ
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
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//ÅäÖÃ³ÉÉÏÀ­£¬Èç¹û°å×ÓÉÏÃ»ÓĞ°ÑÕâÁ½¸ùÏßÉÏÀ­µÄ»°¡£
	GPIO_Init(GPIO_I2C,&GPIO_InitStructure);
}


/*******************************************************************************
**Function Name: 
**Description  :  ²úÉúÆğÊ¼ĞÅºÅ
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
**Description  : ²úÉúÍ£Ö¹ĞÅºÅ 
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
**Description  : Ö÷»ú²úÉúÓ¦´ğĞÅºÅACK 
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
**Description  :  Ö÷»ú²»²úÉúÓ¦´ğĞÅºÅNACK
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
**Description  :  µÈ´ı´Ó»úÓ¦´ğĞÅºÅº
**InPut        :NONE
**OutPut       :NONE
**Returns      :1½ÓÊÕÓ¦´ğÊ§°Ü  0 ½ÓÊÕÓ¦´ğ³É¹¦
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

	while(GPIO_ReadInputDataBit(GPIO_I2C,I2C_SDA)) //SDA±ä³ÉµÍµçÆ½µÄÊ±ºò£¬²Å»á¸øÒ»¸öÓ¦´ğ£¬Èç¹û¶Áµ½Îª0ËµÃ÷¸ø·´À¡ĞÅºÅÁË¡£
	{
		tempTime++; //¼ÆÊıÆ÷£¬Èç¹ûÃ»ÓĞÓ¦´ğ²»ÄÜÔÚÕâÀïÒ»Ö±µÈ¡£
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
**Description  : I2C ·¢ËÍÒ»¸ö×Ö½Ú 
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
	I2C_SCL_L;//À­µÍÊ±ÖÓ¿ªÊ¼Êı¾İ´«Êä

	for(i=0;i<8;i++)
	{
		if((txd&0x80)>0) //0x80  1000 0000 ¼´·¢ËÍÊı¾İÊ±Îª1¾ÍÖ´I2C_SDA_H¡£·ñÔò·¢¸öµÍµçÆ½¡£¸ßÎ»¿ªÊ¼ÏÈ·¢ËÍ
			I2C_SDA_H;
		else
			I2C_SDA_L;

		txd<<=1;
		I2C_SCL_H;//Îª¸ßµçÆ½Ê±²ÅÄÜÕæÕıµÄ·¢ËÍ¡£
		delay_us(2); //·¢ËÍÊı¾İ
		I2C_SCL_L;  //×¼±¸ÏÂÒ»´Î·¢ËÍ
		delay_us(2);
	}
}


/*******************************************************************************
**Function Name: 
**Description  :I2C ¶ÁÈ¡Ò»¸ö×Ö½Ú  
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
   		I2C_SCL_L;//´Ó»ú×¼±¸
		delay_us(2);
		I2C_SCL_H;//Òª¿ªÊ¼¶ÁÁË
		receive<<=1;
		if(GPIO_ReadInputDataBit(GPIO_I2C,I2C_SDA)) //¸ßµçÆ½Ê±£¬ÅĞ¶Ï×ÜÏß×´Ì¬
		   receive++;//Ö´1²Ù×÷¡£
		delay_us(1);	
   }

   	if(ack==0) //ËµÃ÷²»ÓÃ¸øÓ¦´ğ
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
	
	if(EE_TYPE>AT24C16) //µØÖ·¾Í³ÉÁËË«×Ö½ÚÑ°Ö·ÁË¡£
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//·¢ËÍÊı¾İµØÖ·¸ßÎ»
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//Æ÷¼şµØÖ·+Êı¾İµØÖ·
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//Ë«×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»		
				//µ¥×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»			
	I2C_Wait_Ack();

	I2C_Start();
	I2C_Send_Byte(0xA1);//Æ÷¼şµØÖ·£¬ÕâÀïÖ»ÊÇ¸ÄÎªÕæµÄ¶ÁÁË£¬¼´¸Ä±äÏÂ·½Ïò£¬¼´ÏÈĞ´ºó¶Á
	I2C_Wait_Ack();

	temp=I2C_Read_Byte(0); //  0   ´ú±í NACK
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
		I2C_Send_Byte(addr>>8);	//·¢ËÍÊı¾İµØÖ·¸ßÎ»
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//Æ÷¼şµØÖ·+Êı¾İµØÖ·
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//Ë«×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»		
							//µ¥×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»
	I2C_Wait_Ack();

	I2C_Start();
	I2C_Send_Byte(0xA1+((addr/256)<<1));
	I2C_Wait_Ack();

	temp=I2C_Read_Byte(1); //  1   ´ú±í ACK
	temp<<=8;
	temp|=I2C_Read_Byte(0); //  0  ´ú±í NACK£¬¼´¶ÁÁ½¸ö×Öºó¾Í·¢NACK,»¹¿ÉÒÔ¶Á3£¬4¸ö×Ö½ÚµÈ¡£

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
		I2C_Send_Byte(addr>>8);	//·¢ËÍÊı¾İµØÖ·¸ßÎ»
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//Æ÷¼şµØÖ·+Êı¾İµØÖ·
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//Ë«×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»		
				//µ¥×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»
	I2C_Wait_Ack();

	I2C_Send_Byte(dt);//·¢ËÍÂ¦¾İ
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
		I2C_Send_Byte(addr>>8);	//·¢ËÍÊı¾İµØÖ·¸ßÎ»
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//Æ÷¼şµØÖ·+Êı¾İµØÖ·
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//Ë«×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»		
				//µ¥×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»
	I2C_Wait_Ack();

	I2C_Send_Byte(dt>>8);
	I2C_Wait_Ack();

	I2C_Send_Byte(dt&0xFF);//·¢µÍ8Î»Õâ×Ö½ÚÊı¾İ£¬·¢Íêºó¾ÍÍ£Ö¹¡£
	I2C_Wait_Ack();

	I2C_Stop();

	delay_xms(10);
}
/*
	º¯ÊıÃû³Æ£ºvoid AT24Cxx_WriteSerialByte(uint32_t addr,uint8_t *pbuffer,uint16_t len)
	º¯ÊıÊäÈë£ºaddrµØÖ·£¬pbufferĞ´Êı¾İµÄ´æ´¢¿ªÊ¼µØÖ·£¬lenĞ´ÈëÊı¾İµÄ³¤¶È
	º¯Êı¹¦ÄÜ£ºÁ¬ĞøĞ´ÈëAT24CXXº¯Êı
*/

void AT24Cxx_WriteSerialByte(uint32_t addr,uint8_t *pbuffer,uint16_t len)
{
	uint16_t i,j;
	for(j=0;j<((len-1)/8+1);j++)    //°Ë¸ö×Ö½Ú½øĞĞÒ»´ÎµØÖ·ÖØĞÂĞ´Èë
	{
		I2C_Start();
		if(EE_TYPE<AT24C16)
		{
			I2C_Send_Byte(0xA0);
			I2C_Wait_Ack();
			I2C_Send_Byte((addr+j*8)>>8);	//·¢ËÍÊı¾İµØÖ·¸ßÎ»
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
				I2C_Send_Byte(0xA0+(((addr+j*8)/256)<<1));//Æ÷¼şµØÖ·+Êı¾İµØÖ·
			}
		}

		I2C_Wait_Ack();
		I2C_Send_Byte((addr+j*8)%256);//Ë«×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»		
					//µ¥×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»
		I2C_Wait_Ack();
		if(j < (len/8))   //ÅĞ¶ÏÊ£Óà×Ö½ÚÊÇ·ñ¹»8¸ö×Ö½Ú
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
			for(i=0;i<len%8;i++)  //²»¹»8¸ö×Ö½Ú»òÊ£Óà8¸ö×Ö½ÚµÄ´¦Àí
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
	º¯ÊıÃû³Æ£ºvoid AT24Cxx_ReadSerialByte(uint16_t addr,uint8_t *pbuffer,uint16_t len)
	º¯ÊıÊäÈë£ºaddr¶ÁÈ¡µÄÊ×µØÖ·£¬pbuffer´æ´¢µÄÊ×µØÖ·£¬len¶ÁÈ¡Êı¾İ³¤¶È
	º¯Êı¹¦ÄÜ£º´Óaddr¿ªÊ¼¶ÁÈ¡len³¤¶È¸ö×Ö½Ú
	º¯Êı±¸×¢£ºAT24CXXÔÚĞ´Èë¹ı³ÌÖĞ£¬Ò»´ÎĞÔĞ´Èë³¤¶ÈÓĞÒªÇó£¬¶ÁÈ¡Ã»ÓĞÒªÇó
*/

void AT24Cxx_ReadSerialByte(uint16_t addr,uint8_t *pbuffer,uint16_t len)
{
	uint16_t i;

	I2C_Start();
	
	if(EE_TYPE<AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//·¢ËÍÊı¾İµØÖ·¸ßÎ»
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
			I2C_Send_Byte(0xA0+((addr/256)<<1));//Æ÷¼şµØÖ·+Êı¾İµØÖ·
		}
	}

	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//Ë«×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»		
							//µ¥×Ö½ÚÊÇÊı¾İµØÖ·µÍÎ»
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
		pbuffer[i]=I2C_Read_Byte(1); //  1   ´ú±í ACK
	}
	pbuffer[i]=I2C_Read_Byte(0); //  0  ´ú±í NACK£¬¼´¶ÁÁ½¸ö×Öºó¾Í·¢NACK,»¹¿ÉÒÔ¶Á3£¬4¸ö×Ö½ÚµÈ¡£

	I2C_Stop();
	
}
