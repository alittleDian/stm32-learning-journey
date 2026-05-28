#include "stm32f10x.h"

void initOnBoardLed(void);
void initButton(void);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	initOnBoardLed();
	initButton();
	while(1)
	{
	}
}

void initOnBoardLed(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio_init_struct = {0};
	gpio_init_struct.GPIO_Pin = GPIO_Pin_13;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_OD;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &gpio_init_struct); 
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}

void initButton(void)
{
	// # init io pin
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef gpio_init_struct = {0};
	gpio_init_struct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_IPU;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio_init_struct);

	// # init EXTI
	// select
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);
	
	// init line
	EXTI_InitTypeDef exti_init_struct = {0};
	
	exti_init_struct.EXTI_Line = EXTI_Line5;
	exti_init_struct.EXTI_LineCmd = ENABLE;
	exti_init_struct.EXTI_Mode = EXTI_Mode_Interrupt; 
	exti_init_struct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&exti_init_struct);
	
	exti_init_struct.EXTI_Line = EXTI_Line6;
	exti_init_struct.EXTI_LineCmd = ENABLE;
	exti_init_struct.EXTI_Mode = EXTI_Mode_Interrupt; 
	exti_init_struct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&exti_init_struct);
	
	// # init interrupt
	NVIC_InitTypeDef nvic_init_struct = {0};
	nvic_init_struct.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
	nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init_struct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic_init_struct);
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line5) == SET){
		EXTI_ClearFlag(EXTI_Line5);
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	} 
	
	if(EXTI_GetFlagStatus(EXTI_Line6) == SET){
		EXTI_ClearFlag(EXTI_Line6);
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	}
}
