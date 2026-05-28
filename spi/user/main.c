#include "stm32f10x.h"
#include "button.h"

Button_TypeDef button;

void initSpi(void);
void spiMasterSendAndReceive(SPI_TypeDef *spi_x, const uint8_t *data_tx, uint8_t *data_rx, uint16_t size);
void w25q64SaveByte(uint8_t byte);
uint8_t w25q64LoadByte(void);
void initOnBoardLed(void);
void initButton(void);
void buttonClickCallback(uint8_t clicks);

int main(void)
{
	initSpi();
	initOnBoardLed();
	initButton();
	
	uint8_t light_status = w25q64LoadByte();
	if(light_status == 0){
		GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
	}else {
		GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
	}
	
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
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);

	GPIO_InitTypeDef gpio_init_struct = {0};

	// pb3 -> SCK AF_PP 2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_3;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpio_init_struct);
	
	// pb4 -> MISO OUT_IPU
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_4;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &gpio_init_struct);
	
	// pb5 -> MOSI AF_PP 2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_5;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpio_init_struct);
	
	// pa15 -> common_io OUT_PP 2MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpio_init_struct.GPIO_Pin = GPIO_Pin_15;
	gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio_init_struct);
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET);
	
	// # enable SPI module
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	SPI_InitTypeDef spi_init_struct = {0};
	spi_init_struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi_init_struct.SPI_Mode = SPI_Mode_Master;
	spi_init_struct.SPI_DataSize = SPI_DataSize_8b;
	spi_init_struct.SPI_CPOL = SPI_CPOL_Low;
	spi_init_struct.SPI_CPHA = SPI_CPHA_1Edge;
	spi_init_struct.SPI_NSS = SPI_NSS_Soft;
	spi_init_struct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	spi_init_struct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPI1, &spi_init_struct);
	
	// software NSS
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
	
}

void spiMasterSendAndReceive(SPI_TypeDef *spi_x, const uint8_t *data_tx, uint8_t *data_rx, uint16_t size)
{
	// enable
	SPI_Cmd(spi_x, ENABLE);
	
	// send first byte
	SPI_I2S_SendData(spi_x, data_tx[0]);
	
	// send and receive (size - 1) bytes
	for(uint16_t i = 0; i < size - 1; ++i){
		// wait TX empty
		while(SPI_I2S_GetFlagStatus(spi_x, SPI_I2S_FLAG_TXE) != SET);
		
		// send byte
		SPI_I2S_SendData(spi_x, data_tx[i + 1]);
		
		// wait RX not empty
		while(SPI_I2S_GetFlagStatus(spi_x, SPI_I2S_FLAG_RXNE) != SET);
		
		// receive byte
		data_rx[i] =  SPI_I2S_ReceiveData(spi_x);
	}
	
	// receive last byte
	while(SPI_I2S_GetFlagStatus(spi_x, SPI_I2S_FLAG_RXNE) != SET);
	data_rx[size - 1] =  SPI_I2S_ReceiveData(spi_x);
	
	// disable
	SPI_Cmd(spi_x, DISABLE);
}

void w25q64SaveByte(uint8_t byte)
{
	uint8_t buffer[10];
	
	// # write enable
	buffer[0] = 0x06; // write enable command
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); 		// NSS = 0
	spiMasterSendAndReceive(SPI1, buffer, buffer, 1); // send command
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); 			// NSS = 1
	
	// # erase
	buffer[0] = 0x20; // erase command 
	buffer[1] = 0x00; // addr
	buffer[2] = 0x00; // addr
	buffer[3] = 0x00; // addr
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); 		// NSS = 0
	spiMasterSendAndReceive(SPI1, buffer, buffer, 4); // send command
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); 			// NSS = 1
	
	// # wait free
	while(1){
		// request BUSY flag
		GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); 		// NSS = 0
		buffer[0] = 0x05; 																// request BUSY flag command
		spiMasterSendAndReceive(SPI1, buffer, buffer, 1); // send command
		buffer[0] = 0xff; 																// dummy data 
		spiMasterSendAndReceive(SPI1, buffer, buffer, 1); // receive BUSY falg
		GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); 			// NSS = 1
		
		if((buffer[0] & 0x01) == 0){
			break;
		}
	}
	
	// # write enable
	buffer[0] = 0x06; // write enable command
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); 		// NSS = 0
	spiMasterSendAndReceive(SPI1, buffer, buffer, 1); // send command
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); 			// NSS = 1
	
	
	// # page program
	buffer[0] = 0x02; // page program command
	buffer[1] = 0x00; // addr
	buffer[2] = 0x00; // addr
	buffer[3] = 0x00; // addr 
	buffer[4] = byte; // data
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); 		// NSS = 0
	spiMasterSendAndReceive(SPI1, buffer, buffer, 5); // send command
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); 			// NSS = 1
	
	// # wait free
	while(1){
		// request BUSY flag
		GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); 		// NSS = 0
		buffer[0] = 0x05; 																// request BUSY flag command
		spiMasterSendAndReceive(SPI1, buffer, buffer, 1); // send command
		buffer[0] = 0xff; 																// dummy data 
		spiMasterSendAndReceive(SPI1, buffer, buffer, 1); // receive BUSY falg
		GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); 			// NSS = 1
		
		if((buffer[0] & 0x01) == 0){
			break;
		}
	}
}

uint8_t w25q64LoadByte(void)
{
	uint8_t buffer[10];
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_RESET); 		// NSS = 0
	
	// send command
	buffer[0] = 0x03; // load data command
	buffer[1] = 0x00; // addr
	buffer[2] = 0x00; // addr
	buffer[3] = 0x00; // addr 
	spiMasterSendAndReceive(SPI1, buffer, buffer, 4); 
	
	// load data
	buffer[0] = 0xff;
	spiMasterSendAndReceive(SPI1, buffer, buffer, 1); 
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Bit_SET); 			// NSS = 1
	
	return buffer[0]; 
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
			w25q64SaveByte(0x01);
		}else {
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
			w25q64SaveByte(0x00);
		}
	}
}
