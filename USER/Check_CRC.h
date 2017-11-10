#ifndef __CHECK_CRC_H
#define __CHECK_CRC_H

#include "sys.h"

uint16_t MBCrcCheck(uint8_t *puchMsg, uint16_t usDataLen);
uint8_t ReceCrcCheck(uint8_t *pbuff,uint8_t len);

#endif
