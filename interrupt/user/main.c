#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"

uint32_t blink_interval = 1000; // ms

void initOnBoardLed(void);
void initUsart(void);

int main(void)
{
	// interrupt config
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	initOnBoardLed();
	initUsart();
	
	My_USART_Printf(USART1, "Send {0,1,2} to adjust led blink interval. \r\n"); 
	
	while(1)
	{
		// on
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
		Delay(blink_interval);
	
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
		Delay(blink_interval);
	}
}

void initOnBoardLed(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio_init_struct = {0};
	gpio_init_struct.GPIO_Pin = GPIO_Pin_13;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_OD;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio_init_struct);
}

void initUsart(void)
{
	// # init io pin
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef gpio_init_struct = {0};
	
	gpio_init_struct.GPIO_Pin = GPIO_Pin_9;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio_init_struct);
	
	gpio_init_struct.GPIO_Pin = GPIO_Pin_10;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_IPU;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio_init_struct);

	// # init uart
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	USART_InitTypeDef usart_init_struct = {0};
	usart_init_struct.USART_BaudRate = 115200;
	usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_init_struct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	usart_init_struct.USART_Parity = USART_Parity_No;
	usart_init_struct.USART_StopBits = USART_StopBits_1;
	usart_init_struct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &usart_init_struct);
	
	USART_Cmd(USART1, ENABLE);
	
	// # interrupt config
	NVIC_InitTypeDef nvic_init_struct = {0};
	nvic_init_struct.NVIC_IRQChannel = USART1_IRQn;
	nvic_init_struct.NVIC_IRQChannelCmd = ENABLE;
	nvic_init_struct.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init_struct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic_init_struct);
}

void USART1_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != SET){
		return;
	}
	
	uint8_t data = USART_ReceiveData(USART1);
	
	if(data == '0'){
		blink_interval = 1000;
	} else if (data == '1'){
		blink_interval = 200;
	} else if (data == '2'){
		blink_interval = 50;
	}
}
