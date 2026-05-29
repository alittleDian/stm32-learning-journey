#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"

void initUsart(void);

int main(void)
{
	initUsart();
	My_USART_Printf(USART1, "Use HCSR04 to measure distance. \r\n");
	
	while(1)
	{
	}
}

void initUsart(void)
{
	// # init io pin
	GPIO_InitTypeDef gpio_init_struct;
	// pa9 tx
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_9;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio_init_struct);
	// pa10 rx
	gpio_init_struct.GPIO_Pin = GPIO_Pin_10;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &gpio_init_struct);
	
	// # init usart module
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef usart_init_strtuct;
	usart_init_strtuct.USART_BaudRate = 115200;
	usart_init_strtuct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_strtuct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart_init_strtuct.USART_Parity = USART_Parity_No;
	usart_init_strtuct.USART_StopBits = USART_StopBits_1;
	usart_init_strtuct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &usart_init_strtuct);
	// enable
	USART_Cmd(USART1, ENABLE);
}
