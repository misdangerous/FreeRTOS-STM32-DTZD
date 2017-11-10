
#include "switch_io.h"

void SwitchIO_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC \
						| RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF, ENABLE);	 
	
	/*设置输入引脚*/
	GPIO_InitStructure.GPIO_Pin = SIGNAL_INPUT_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_INPUT_1, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SIGNAL_INPUT_2;
	GPIO_Init(GPIO_INPUT_2, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SIGNAL_INPUT_3;
	GPIO_Init(GPIO_INPUT_3, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SIGNAL_INPUT_4;
	GPIO_Init(GPIO_INPUT_4, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SIGNAL_INPUT_5;
	GPIO_Init(GPIO_INPUT_5, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SIGNAL_INPUT_6;
	GPIO_Init(GPIO_INPUT_6, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SIGNAL_INPUT_7;
	GPIO_Init(GPIO_INPUT_7, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = SIGNAL_INPUT_8;
//	GPIO_Init(GPIO_INPUT_8, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = SIGNAL_INPUT_9;
//	GPIO_Init(GPIO_INPUT_9, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = SIGNAL_INPUT_10;
//	GPIO_Init(GPIO_INPUT_10, &GPIO_InitStructure);
	
	/*设置输出模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = SIGNAL_OUTPUT_1;
	GPIO_Init(GPIO_OUTPUT_1, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SIGNAL_OUTPUT_2;
	GPIO_Init(GPIO_OUTPUT_2, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SIGNAL_OUTPUT_3;
	GPIO_Init(GPIO_OUTPUT_3, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SIGNAL_OUTPUT_4;
	GPIO_Init(GPIO_OUTPUT_4, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SIGNAL_OUTPUT_5;
	GPIO_Init(GPIO_OUTPUT_5, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIO_OUTPUT_1,SIGNAL_OUTPUT_1);
	GPIO_SetBits(GPIO_OUTPUT_2,SIGNAL_OUTPUT_2);
	GPIO_SetBits(GPIO_OUTPUT_3,SIGNAL_OUTPUT_3);
	GPIO_SetBits(GPIO_OUTPUT_4,SIGNAL_OUTPUT_4);
	GPIO_SetBits(GPIO_OUTPUT_5,SIGNAL_OUTPUT_5);
}


