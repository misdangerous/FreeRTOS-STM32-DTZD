#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//DS18B20��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved										  
//////////////////////////////////////////////////////////////////////////////////
//���������ϵ�DS18B20��������MAXNUM
#define MAXNUM 20

//DS18B20ָ��
typedef enum   
{  
    SEARCH_ROM          =   0xf0,   //����ROMָ��  
    READ_ROM            =   0x33,   //��ȡROMָ��
    MATH_ROM            =   0x55,   //ƥ��ROMָ��
    SKIP_ROM            =   0xcc,   //����ROMָ��
    ALARM_SEARCH        =   0xec,   //��������ָ�� 
    CONVERT_T           =   0x44,   //�¶�ת��ָ��
    WRITE_SCRATCHPAD    =   0x4e,   //д�ݴ���ָ��
    READ_SCRATCHPAD     =   0xbe,   //��ȡת����ָ��
    COPY_SCRATCHPAD     =   0x48,   //�����ݴ���ָ��  
    RECALL_E2           =   0xb8,   //�ٻ�EEPROMָ��
    READ_POWER_SUPPLY   =   0xb4,   //��ȡ��Դģʽָ��  
} DS18B20_CMD;  
  
  
  
//DS18B20 ROM���� 
typedef struct  
{  
    u8  DS18B20_CODE;   //DS18B20�����߱���:0x19  
    u8  SN_1;           //���кŵ�1�ֽ�  
    u8  SN_2;           //���кŵ�2�ֽ�
    u8  SN_3;           //���кŵ�3�ֽ�
    u8  SN_4;           //���кŵ�4�ֽ�  
    u8  SN_5;           //���кŵ�5�ֽ� 
    u8  SN_6;           //���кŵ�6�ֽ�
    u8  crc8;           //CRC8У��    
} DS18B20_ROM_CODE;   
  
#define TRUE 0x00
#define FALSE 0Xff

////IO��������
#define DS18B20_IO_IN()  {GPIOC->CRL&=0XFFFFFF0F;GPIOC->CRL|=1<<7;}	    //PB10����ģʽ
#define DS18B20_IO_OUT() {GPIOC->CRL&=0XFFFFFF0F;GPIOC->CRL|=3<<4;} 	//PB10���ģʽ
 
////IO��������											   
#define	DS18B20_DQ_OUT PCout(1) //���ݶ˿�	PC1
#define	DS18B20_DQ_IN  PCin(1)  //���ݶ˿�	PC1 
   	
u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20    


u8 DS18B20_SearchROM(u8 (*pID)[8],u8 Num);
s16 DS18B20_ReadDesignateTemper(u8 pID[8]);

#endif















