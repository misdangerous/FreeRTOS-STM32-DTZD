#ifndef __EEPROM_SAVE_H
#define __EEPROM_SAVE_H

#include "sys.h"
#include "sense_modbus.h"
#include "ds18b20.h"
#include "Switch_Task.h"

/*
*保存探头地址的位置
*/
#define Machine_Save_Addr      0
#define Current_Save_Addr      (Machine_Save_Addr+64)
#define InfraredTemp_Save_Addr (Current_Save_Addr+64)
#define Vibrator_Save_Addr     (InfraredTemp_Save_Addr+64)
#define DS18B20_Save_Addr      (Vibrator_Save_Addr+64)

#define Machine_SaveAddr_Len       2+2
#define Current_SaveAddr_Len       (2*CURRENT_NUM)+2 //最大32个电流地址
#define InfraredTemp_SaveAddr_Len  (2*INFRAREDTEMP_NUM)+2
#define Vibrator_SaveAddr_Len      (2*VIBRATOR_NUM)+2
#define DS18B20_SaveAddr_Len       (MAXNUM*8)+2
/*
*保存上报时间的位置及长度
*/
#define Current_Save_SendTime      (DS18B20_Save_Addr+256)//DS18B20长度过长，造成长度加长
#define InfraredTemp_Save_SendTime (Current_Save_SendTime+64)
#define Vibrator_Save_SendTime     (InfraredTemp_Save_SendTime+64)
#define HeartBeat_Save_SendTime    (Vibrator_Save_SendTime+64)
#define Switch_Save_SendTime       (HeartBeat_Save_SendTime+64)
#define DS18B20_Save_SendTime      (Switch_Save_SendTime+64)

#define Switch_SaveSendTime_Len        2+2
#define Current_SaveSendTime_Len       2+2
#define InfraredTemp_SaveSendTime_Len  2+2
#define Vibrator_SaveSendTime_Len      2+2
#define DS18B20_SaveSendTime_Len       2+2
#define HeartBeat_SaveSendTime_Len     2+2

typedef enum 
{
	EEPROM_Machine_Addr,
	EEPROM_Current_Addr,
	EEPROM_InfraredTemp_Addr,
	EEPROM_Vibrator_Addr,
	EEPROM_DS18B20_Addr,
	EEPROM_DS18B20_SendTime,
	EEPROM_Current_SendTime,
	EEPROM_InfraredTemp_SendTime,
	EEPROM_Vibrator_SendTime,
	EEPROM_Switch_SendTime,
	EEPROM_HeartBeat_SendTime,
}EEPROM_Save_TypeDef;


uint8_t EEPROM_Get_Addr(EEPROM_Save_TypeDef function,uint8_t *pBuffer,uint8_t *Getlen);
uint8_t EEPROM_Change_Addr(EEPROM_Save_TypeDef function,uint8_t *pBuffer,uint8_t len);
void InitMachineAddr(uint16_t *pMachineAddr);
#endif

