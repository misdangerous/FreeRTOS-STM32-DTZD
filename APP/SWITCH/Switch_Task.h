#ifndef __SWITCH_TASK_H
#define __SWITCH_TASK_H

#include "sys.h"

#define SWITCH_OUTPUT_NUM 5
#define SWITCH_INPUT_NUM  7

typedef enum
{
	Switch_ON,
	Switch_OFF,
}Switch_TypeDef;

struct Switch_Struct
{
	Switch_TypeDef IN[SWITCH_INPUT_NUM];
	Switch_TypeDef OUT[SWITCH_OUTPUT_NUM];
};

extern struct Switch_Struct Switch_Struction;

#endif
