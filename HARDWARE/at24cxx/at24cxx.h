#ifndef __APP_AT24CXX_H
#define __APP_AT24CXX_H


#include "stm32f10x.h"


//如果移植程序时只要改一下三个地方就行了
#define I2C_SCL GPIO_Pin_6
#define I2C_SDA GPIO_Pin_7
#define GPIO_I2C GPIOB

#define I2C_SCL_H GPIO_SetBits(GPIO_I2C,I2C_SCL)
#define I2C_SCL_L GPIO_ResetBits(GPIO_I2C,I2C_SCL)

#define I2C_SDA_H GPIO_SetBits(GPIO_I2C,I2C_SDA)
#define I2C_SDA_L GPIO_ResetBits(GPIO_I2C,I2C_SDA)

#define AT24C01  127 //0开始的，共有128个字节
#define AT24C02  255
#define AT24C04  511
#define AT24C08  1023
#define AT24C16  2047
#define AT24C32  4095
#define AT24C64  8191
#define AT24C128 16383
#define AT24C256 32767 //64字节一页,地址选取注意

#define EE_TYPE  AT24C256

uint8_t AT24Cxx_ReadOneByte(uint16_t addr);
uint16_t AT24Cxx_ReadTwoByte(uint16_t addr);
void AT24Cxx_WriteOneByte(uint16_t addr,uint8_t dt);
void AT24Cxx_WriteTwoByte(uint16_t addr,uint16_t dt);
void AT24Cxx_WriteSerialByte(uint32_t addr,uint8_t *pbuffer,uint16_t len);
void AT24Cxx_ReadSerialByte(uint16_t addr,uint8_t *pbuffer,uint16_t len);

void I2C_InitCfg(void);
void I2C_SDA_OUT(void);
void I2C_SDA_IN(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NAck(void);
uint8_t   I2C_Wait_Ack(void);
void I2C_Send_Byte(uint8_t txd);
uint8_t   I2C_Read_Byte(uint8_t ack);

#endif
