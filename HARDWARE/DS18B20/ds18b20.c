#include "ds18b20.h"
#include "delay.h"	
#include <string.h>
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//DS18B20驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved										  
//////////////////////////////////////////////////////////////////////////////////
 

 
//复位DS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); //SET PG11 OUTPUT
  DS18B20_DQ_OUT=0; //拉低DQ
  delay_us(750);    //拉低750us
  DS18B20_DQ_OUT=1; //DQ=1 
	delay_us(15);     //15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
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
//从DS18B20读取一个位
//返回值：1/0
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
//从DS18B20读取一个字节
//返回值：读到的数据
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
//写一个字节到DS18B20
//dat：要写入的字节
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
//开始温度转换
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	   
	  DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
u8 DS18B20_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能GPIOB时钟

  //GPIOC1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//普通输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
 
 	DS18B20_Rst();
	return DS18B20_Check();
}  
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
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
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(double)tem*0.625;//转换     
	if(temp)return tem; //返回温度值
	else return -tem;    
}

/************************************************************************************************************************* 
*函数名:void DS18B20_WriteBit(u8 bit)   
*功能  :向DS18B20写入1bit的数据 
*参数  :bit 
*返回  :无
*作者  :Zong_Yang@outlook.com 
*************************************************************************************************************************/  
void DS18B20_WriteBit(u8 bit)  
{  
    DS18B20_IO_OUT();    //设置数据总线接口为输出
    DS18B20_DQ_OUT=0;      //拉低数据总线
    delay_us(12);				//延时10-15us  
    DS18B20_DQ_OUT= bit&0x01;   //写入数据,
    delay_us(30);				//延时20-45us  
    DS18B20_DQ_OUT=1;			//释放总线
    delay_us(5);  
}  

/************************************************************************************************************************* 
*函数名:u8 DS18B20_Read2Bit(void)    
*功能  :读取2bit数据
*参数  :无 
*返回  :data
*作者  :Zong_Yang@outlook.com 
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
*函数名:u8 DS18B20_SearchROM(u8 (*pID)[8],u8 Num)   
*功能  :查询DS18B20的ROM
*参数  :(1)、 pIN:总线上DS18B20的ID存储的缓冲区指针
				(2)、 Num:DS18B20的个数，在MAXNUM中有定义
*返回  :搜索到总线上DS18B20的个数
*作者  :Zong_Yang@outlook.com 
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
        DS18B20_Rst();								//复位DS18B20总线  
		DS18B20_Check();
        DS18B20_Write_Byte(SEARCH_ROM);	//搜索ROM      
        for(m=0;m<8;m++)  
        {  
            for(n=0;n<8;n++)  
            {  
                k=DS18B20_Read2Bit();		// 读两位数据
                k=k&0x03;  
                s= s>>1;  
                if(k==0x01)							//0000 0001 如果读到的数据为0
                {             
                    DS18B20_WriteBit(0);//写0，使总线上为0的器件响应  
                    ss[(m*8+n)]=0;  
                }  
                else if(k==0x02)				//0000 0010 如果读到的数据为1
                {  
                    s=s|0x80;  
                    DS18B20_WriteBit (1);//写1，使总线上为1的器件响应    
                    ss[(m*8+n)]=1;  
                }  
                else if(k==0x00)//如果读取到的数据为00，则有冲突，需进行冲突位判断  
                {                
                    ConflictBit=m*8+n+1;                   
                    if(ConflictBit>BUFFER[l])//如果冲突位大于栈顶，则写0   
                    {                         
                        DS18B20_WriteBit (0);  
                        ss[(m*8+n)]=0;                                                
                        BUFFER[++l]=ConflictBit;                         
                    }  
                    else if(ConflictBit<BUFFER[l])//如果冲突位小于栈顶，则写以前的数据 
                    {  
                        s=s|((ss[(m*8+n)]&0x01)<<7);  
                        DS18B20_WriteBit (ss[(m*8+n)]);  
                    }  
                    else if(ConflictBit==BUFFER[l])//如果冲突位等于栈顶，则写1 
                    {  
                        s=s|0x80;  
                        DS18B20_WriteBit (1);  
                        ss[(m*8+n)]=1;  
                        l=l-1;  
                    }  
                }  
                else//如果读到的数据为0x03(0000 0011),则说明单总线上不存在任何设备
                {  
                    return num; //搜索完成，返回搜索到的个数
                }
				delay_us(5);
            }  
            pID[num][m]=s;
						s=0;
        }  
        num=num+1;
    }  
    while(BUFFER[l]!=0&&(num<MAXNUM));   
      
    return num;     //返回搜索到的个数 
}
  
  
/************************************************************************************************************************* 
*函数名:s16 DS18B20_ReadDesignateTemper(u8 pID[8])   
*功能  :读取指定ID的DS18B20的温度值
*参数  :(1)、 pIN:总线上DS18B20的ID
*返回  :温度值*100
*作者  :Zong_Yang@outlook.com 
*************************************************************************************************************************/  

s16 DS18B20_ReadDesignateTemper(u8 pID[8])  
{  
    u8 th, tl;  
    s16 data;  
    DS18B20_Rst();
    if(DS18B20_Check() == FALSE)				//错误判断      
    {  
        return 0xffff;              		//返回错误信息
    }  
  
    DS18B20_Write_Byte(SKIP_ROM);        //跳过ROM指令
    DS18B20_Write_Byte(CONVERT_T);       //启动温度转换 指令
    DS18B20_Rst();  
	DS18B20_Check();
    DS18B20_Write_Byte(MATH_ROM);        //匹配ROM指令
    for(data = 0;data < 8;data ++)   		//发送8个字节的序列号   
    {  
       DS18B20_Write_Byte(pID[data]);  
    }  
    delay_us(10);  
    DS18B20_Write_Byte(READ_SCRATCHPAD);    //读取温度指令 
    tl = DS18B20_Read_Byte();    				//低8位数据
    th = DS18B20_Read_Byte();    				//高8位数据
    data = th; 
		data = data<<8;
    data = data|tl;  
    data = data*6.25;           				//数值精确到小数点后两位  
      
    return data;												//返回值=温度*100;  
}












