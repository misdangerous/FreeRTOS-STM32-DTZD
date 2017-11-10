#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   
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
//定义总线上的DS18B20的最大个数MAXNUM
#define MAXNUM 20

//DS18B20指令
typedef enum   
{  
    SEARCH_ROM          =   0xf0,   //搜索ROM指令  
    READ_ROM            =   0x33,   //读取ROM指令
    MATH_ROM            =   0x55,   //匹配ROM指令
    SKIP_ROM            =   0xcc,   //跳过ROM指令
    ALARM_SEARCH        =   0xec,   //报警搜索指令 
    CONVERT_T           =   0x44,   //温度转换指令
    WRITE_SCRATCHPAD    =   0x4e,   //写暂存器指令
    READ_SCRATCHPAD     =   0xbe,   //读取转存器指令
    COPY_SCRATCHPAD     =   0x48,   //拷贝暂存器指令  
    RECALL_E2           =   0xb8,   //召回EEPROM指令
    READ_POWER_SUPPLY   =   0xb4,   //读取电源模式指令  
} DS18B20_CMD;  
  
  
  
//DS18B20 ROM编码 
typedef struct  
{  
    u8  DS18B20_CODE;   //DS18B20单总线编码:0x19  
    u8  SN_1;           //序列号第1字节  
    u8  SN_2;           //序列号第2字节
    u8  SN_3;           //序列号第3字节
    u8  SN_4;           //序列号第4字节  
    u8  SN_5;           //序列号第5字节 
    u8  SN_6;           //序列号第6字节
    u8  crc8;           //CRC8校验    
} DS18B20_ROM_CODE;   
  
#define TRUE 0x00
#define FALSE 0Xff

////IO方向设置
#define DS18B20_IO_IN()  {GPIOC->CRL&=0XFFFFFF0F;GPIOC->CRL|=1<<7;}	    //PB10输入模式
#define DS18B20_IO_OUT() {GPIOC->CRL&=0XFFFFFF0F;GPIOC->CRL|=3<<4;} 	//PB10输出模式
 
////IO操作函数											   
#define	DS18B20_DQ_OUT PCout(1) //数据端口	PC1
#define	DS18B20_DQ_IN  PCin(1)  //数据端口	PC1 
   	
u8 DS18B20_Init(void);			//初始化DS18B20
short DS18B20_Get_Temp(void);	//获取温度
void DS18B20_Start(void);		//开始温度转换
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);		//读出一个字节
u8 DS18B20_Read_Bit(void);		//读出一个位
u8 DS18B20_Check(void);			//检测是否存在DS18B20
void DS18B20_Rst(void);			//复位DS18B20    


u8 DS18B20_SearchROM(u8 (*pID)[8],u8 Num);
s16 DS18B20_ReadDesignateTemper(u8 pID[8]);

#endif















