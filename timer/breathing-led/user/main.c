#include "stm32f10x.h"
#include "delay.h"
#include "math.h"

void initPwm(void);

int main(void)
{
	initPwm();
	
	while(1)
	{
		float t = GetTick() * 1.0e-3f;
		float duty = 0.5 * (sin(2 * 3.14 * t) + 1);
		uint16_t ccr = duty * 1000;
		
		TIM_SetCompare1(TIM1, ccr);
	}
}

void initPwm(void)
{
	// # init pwm pin
	GPIO_InitTypeDef gpio_init_struct;
	// pa8
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_8;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio_init_struct);
	// pb13
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_13;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpio_init_struct);

	// # init time base
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_TimeBaseInitTypeDef time_base_init_struct;
	time_base_init_struct.TIM_CounterMode = TIM_CounterMode_Up;
	time_base_init_struct.TIM_Prescaler = 71;
	time_base_init_struct.TIM_Period = 999;
	time_base_init_struct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &time_base_init_struct);
	// open ARR preload
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	// enable
	TIM_Cmd(TIM1, ENABLE);
	
	// # init output compare
	TIM_OCInitTypeDef oc_init_struct;
	oc_init_struct.TIM_OCMode = TIM_OCMode_PWM1;
	oc_init_struct.TIM_OutputState = ENABLE;
	oc_init_struct.TIM_OutputNState = ENABLE;
	oc_init_struct.TIM_OCPolarity = TIM_OCPolarity_High;
	oc_init_struct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	oc_init_struct.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &oc_init_struct);
	// open CCR preload
	TIM_CCPreloadControl(TIM1, ENABLE);
	// enable MOE
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}
