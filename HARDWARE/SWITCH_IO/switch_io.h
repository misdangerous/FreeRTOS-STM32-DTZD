#ifndef __SWITCH_IO_H
#define __SWITCH_IO_H

#include "sys.h"

#define GPIO_INPUT_1    GPIOE
#define SIGNAL_INPUT_1  GPIO_Pin_2
#define SWITCH_INPUT_1  PEin(2)

#define GPIO_INPUT_2    GPIOE
#define SIGNAL_INPUT_2  GPIO_Pin_3
#define SWITCH_INPUT_2  PEin(3)

#define GPIO_INPUT_3    GPIOE
#define SIGNAL_INPUT_3  GPIO_Pin_4
#define SWITCH_INPUT_3  PEin(4)

#define GPIO_INPUT_4    GPIOE
#define SIGNAL_INPUT_4  GPIO_Pin_5
#define SWITCH_INPUT_4  PEin(5)

#define GPIO_INPUT_5    GPIOE
#define SIGNAL_INPUT_5  GPIO_Pin_6
#define SWITCH_INPUT_5  PEin(6)

#define GPIO_INPUT_6    GPIOF
#define SIGNAL_INPUT_6  GPIO_Pin_0
#define SWITCH_INPUT_6  PFin(0)

#define GPIO_INPUT_7    GPIOC
#define SIGNAL_INPUT_7  GPIO_Pin_0 
#define SWITCH_INPUT_7  PCin(0)

#define GPIO_INPUT_8    GPIOC
#define SIGNAL_INPUT_8  GPIO_Pin_1
#define SWITCH_INPUT_8  PCin(1)

#define GPIO_INPUT_9    GPIOC
#define SIGNAL_INPUT_9  GPIO_Pin_2
#define SWITCH_INPUT_9  PCin(2)

#define GPIO_INPUT_10   GPIOC
#define SIGNAL_INPUT_10 GPIO_Pin_3
#define SWITCH_INPUT_10 PCin(3)


#define GPIO_OUTPUT_1   GPIOA
#define SIGNAL_OUTPUT_1 GPIO_Pin_6
#define SWITCH_OUTPUT_1 PAout(6)

#define GPIO_OUTPUT_2   GPIOA
#define SIGNAL_OUTPUT_2 GPIO_Pin_7
#define SWITCH_OUTPUT_2 PAout(7)

#define GPIO_OUTPUT_3   GPIOC
#define SIGNAL_OUTPUT_3 GPIO_Pin_4
#define SWITCH_OUTPUT_3 PCout(4)

#define GPIO_OUTPUT_4   GPIOC
#define SIGNAL_OUTPUT_4 GPIO_Pin_5
#define SWITCH_OUTPUT_4 PCout(5)

#define GPIO_OUTPUT_5   GPIOB
#define SIGNAL_OUTPUT_5 GPIO_Pin_0
#define SWITCH_OUTPUT_5 PBout(0)

#define OUTPUT_ON  0
#define OUTPUT_OFF 1

#define INPUT_ON  1
#define INPUT_OFF 0

void SwitchIO_Init(void);

#endif

