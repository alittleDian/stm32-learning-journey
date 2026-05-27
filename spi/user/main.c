#include "stm32f10x.h"
#include "button.h"

Button_TypeDef button;

void initSpi(void);
void w25q64SaveByte(uint8_t byte);
void w25q64LoadByte(void);
void initOnBoardLed(void);
void initButton(void);
void buttonClickCallback(uint8_t clicks);

int main(void)
{
	initSpi();
	initOnBoardLed();
	initButton();
	
	while(1)
	{
		My_Button_Proc(&button);
	}
}

void initSpi(void)
{
	// # enable io pin
	// remap
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);

	GPIO_InitTypeDef gpio_init_struct = {0};

	// pb3 -> sck afio_pp 2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_3;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpio_init_struct);
	
	// pb4 -> miso out_ipu 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_4;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &gpio_init_struct);
	
	// pb5 -> mosi afio_pp 2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_5;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpio_init_struct);
	
	// pa15 -> common_io out_pp 2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_15;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio_init_struct);
	
	// # enable spi module
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
}
void w25q64SaveByte(uint8_t byte)
{
	// write enable
	
	// erase
	
	// wait free
	
	// write enable
	
	// wait free
}

void w25q64LoadByte(void)
{

}

void initOnBoardLed(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef gpio_init_struct = {0};
	gpio_init_struct.GPIO_Pin = GPIO_Pin_13;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_OD;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &gpio_init_struct);
}

void initButton(void)
{
	Button_InitTypeDef button_init_struct = {0};
	button_init_struct.GPIOx = GPIOA;
	button_init_struct.GPIO_Pin = GPIO_Pin_0;
	button_init_struct.button_clicked_cb = buttonClickCallback;
	
	My_Button_Init(&button, &button_init_struct);
}

void buttonClickCallback(uint8_t clicks)
{
	if(clicks == 1){
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET){
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
		}else {
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
		}
	}
}
