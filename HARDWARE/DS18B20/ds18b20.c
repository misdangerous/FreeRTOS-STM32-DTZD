#include "ds18b20.h"
#include "delay.h"	
#include <string.h>
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
 

 
//��λDS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); //SET PG11 OUTPUT
  DS18B20_DQ_OUT=0; //����DQ
  delay_us(750);    //����750us
  DS18B20_DQ_OUT=1; //DQ=1 
	delay_us(15);     //15US
}
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
u8 DS18B20_Check(void) 	   
{   
	u8 retry= 0;
	DS18B20_IO_IN();//SET PG11 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
  u8 data;
	DS18B20_IO_OUT();//SET PG11 OUTPUT
  DS18B20_DQ_OUT=0; 
	delay_us(2);
  DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();//SET PG11 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
  else data=0;	 
  delay_us(50);           
  return data;
}
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
u8 DS18B20_Read_Byte(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT();//SET PG11 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT=0;// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT=1;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT=0;// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}
//��ʼ�¶�ת��
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	   
	  DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ��GPIOBʱ��

  //GPIOC1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
 
 	DS18B20_Rst();
	return DS18B20_Check();
}  
//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
	  short tem;
    DS18B20_Start ();                    // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB   
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//�¶�Ϊ��  
    }else temp=1;//�¶�Ϊ��	  	  
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    tem=(double)tem*0.625;//ת��     
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
}

/************************************************************************************************************************* 
*������:void DS18B20_WriteBit(u8 bit)   
*����  :��DS18B20д��1bit������ 
*����  :bit 
*����  :��
*����  :Zong_Yang@outlook.com 
*************************************************************************************************************************/  
void DS18B20_WriteBit(u8 bit)  
{  
    DS18B20_IO_OUT();    //�����������߽ӿ�Ϊ���
    DS18B20_DQ_OUT=0;      //������������
    delay_us(12);				//��ʱ10-15us  
    DS18B20_DQ_OUT= bit&0x01;   //д������,
    delay_us(30);				//��ʱ20-45us  
    DS18B20_DQ_OUT=1;			//�ͷ�����
    delay_us(5);  
}  

/************************************************************************************************************************* 
*������:u8 DS18B20_Read2Bit(void)    
*����  :��ȡ2bit����
*����  :�� 
*����  :data
*����  :Zong_Yang@outlook.com 
*************************************************************************************************************************/  

u8 DS18B20_Read2Bit(void)  
{  
    u8 i,data = 0;   
    for(i = 0;i < 2;i ++)  
    {  
        data <<= 1;  
        if(DS18B20_Read_Bit())
				{
					data = data|1;
				}					  
    }  
    return data;  
}

/************************************************************************************************************************* 
*������:u8 DS18B20_SearchROM(u8 (*pID)[8],u8 Num)   
*����  :��ѯDS18B20��ROM
*����  :(1)�� pIN:������DS18B20��ID�洢�Ļ�����ָ��
				(2)�� Num:DS18B20�ĸ�������MAXNUM���ж���
*����  :������������DS18B20�ĸ���
*����  :Zong_Yang@outlook.com 
*************************************************************************************************************************/  

u8 DS18B20_SearchROM(u8 (*pID)[8],u8 Num)  
{   
    unsigned char k,l=0,ConflictBit,m,n;  
    unsigned char BUFFER[MAXNUM-1]={0};  
    unsigned char ss[64];
	unsigned char s=0;  
    u8 num = 0;   
    do  
    {  
        DS18B20_Rst();								//��λDS18B20����  
		DS18B20_Check();
        DS18B20_Write_Byte(SEARCH_ROM);	//����ROM      
        for(m=0;m<8;m++)  
        {  
            for(n=0;n<8;n++)  
            {  
                k=DS18B20_Read2Bit();		// ����λ����
                k=k&0x03;  
                s= s>>1;  
                if(k==0x01)							//0000 0001 �������������Ϊ0
                {             
                    DS18B20_WriteBit(0);//д0��ʹ������Ϊ0��������Ӧ  
                    ss[(m*8+n)]=0;  
                }  
                else if(k==0x02)				//0000 0010 �������������Ϊ1
                {  
                    s=s|0x80;  
                    DS18B20_WriteBit (1);//д1��ʹ������Ϊ1��������Ӧ    
                    ss[(m*8+n)]=1;  
                }  
                else if(k==0x00)//�����ȡ��������Ϊ00�����г�ͻ������г�ͻλ�ж�  
                {                
                    ConflictBit=m*8+n+1;                   
                    if(ConflictBit>BUFFER[l])//�����ͻλ����ջ������д0   
                    {                         
                        DS18B20_WriteBit (0);  
                        ss[(m*8+n)]=0;                                                
                        BUFFER[++l]=ConflictBit;                         
                    }  
                    else if(ConflictBit<BUFFER[l])//�����ͻλС��ջ������д��ǰ������ 
                    {  
                        s=s|((ss[(m*8+n)]&0x01)<<7);  
                        DS18B20_WriteBit (ss[(m*8+n)]);  
                    }  
                    else if(ConflictBit==BUFFER[l])//�����ͻλ����ջ������д1 
                    {  
                        s=s|0x80;  
                        DS18B20_WriteBit (1);  
                        ss[(m*8+n)]=1;  
                        l=l-1;  
                    }  
                }  
                else//�������������Ϊ0x03(0000 0011),��˵���������ϲ������κ��豸
                {  
                    return num; //������ɣ������������ĸ���
                }
				delay_us(5);
            }  
            pID[num][m]=s;
						s=0;
        }  
        num=num+1;
    }  
    while(BUFFER[l]!=0&&(num<MAXNUM));   
      
    return num;     //�����������ĸ��� 
}
  
  
/************************************************************************************************************************* 
*������:s16 DS18B20_ReadDesignateTemper(u8 pID[8])   
*����  :��ȡָ��ID��DS18B20���¶�ֵ
*����  :(1)�� pIN:������DS18B20��ID
*����  :�¶�ֵ*100
*����  :Zong_Yang@outlook.com 
*************************************************************************************************************************/  

s16 DS18B20_ReadDesignateTemper(u8 pID[8])  
{  
    u8 th, tl;  
    s16 data;  
    DS18B20_Rst();
    if(DS18B20_Check() == FALSE)				//�����ж�      
    {  
        return 0xffff;              		//���ش�����Ϣ
    }  
  
    DS18B20_Write_Byte(SKIP_ROM);        //����ROMָ��
    DS18B20_Write_Byte(CONVERT_T);       //�����¶�ת�� ָ��
    DS18B20_Rst();  
	DS18B20_Check();
    DS18B20_Write_Byte(MATH_ROM);        //ƥ��ROMָ��
    for(data = 0;data < 8;data ++)   		//����8���ֽڵ����к�   
    {  
       DS18B20_Write_Byte(pID[data]);  
    }  
    delay_us(10);  
    DS18B20_Write_Byte(READ_SCRATCHPAD);    //��ȡ�¶�ָ�� 
    tl = DS18B20_Read_Byte();    				//��8λ����
    th = DS18B20_Read_Byte();    				//��8λ����
    data = th; 
		data = data<<8;
    data = data|tl;  
    data = data*6.25;           				//��ֵ��ȷ��С�������λ  
      
    return data;												//����ֵ=�¶�*100;  
}












