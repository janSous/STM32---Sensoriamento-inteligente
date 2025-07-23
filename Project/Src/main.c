#include "stm32f1xx.h"
#include <stdint.h>
#include <math.h>

static uint8_t rx_buffer[6];
static uint8_t tx_buffer[3];

float temp = 0;
float umid = 0;
int i_temp = 0;
int i_umid = 0;

//Comandos AHT10
#define AHT10_INIT		0xE1
#define AHT10_TRIG      0xAC
#define AHT10_RESET     0XBA

void aht10_Sync(void);
void aht10_Write(uint8_t*, uint8_t);
void aht10_Read(uint8_t*, uint8_t);
void aht10_getValue(void);
void delay_us(uint32_t us);

int main(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
	GPIOB->CRL |= GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7; //PB6-PB8 OUTPUT 50MHZ OPEN-DRAIN

	//habilita i2c
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	I2C1->CR1 |= (1 << 15);
	I2C1->CR1 &= ~(1 << 15);

	I2C1->CR2 |= 8;

	I2C1->CCR |= 40; //Frequencia do SCL (100KHz)
	I2C1->TRISE |= 9; //Tempo maximo de subida

	I2C1->CR1 |= (1<<0); //Habilita o i2c
	I2C1->CR1 |= (1 << 10); //Aciona ack
	I2C1->CR2 |= (1 << 11);
	I2C1->CR2 |= (1 << 12);


	//configura DMA para o I2C
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	//Configurando Canal RX DMA
	DMA1_Channel7->CPAR = (uint32_t)(&(I2C1->DR));
	DMA1_Channel7->CMAR = (uint32_t)rx_buffer;
	DMA1_Channel7->CNDTR = 6;
	DMA1_Channel7->CCR |= (1 << 7); //Memoria incrementa
	DMA1_Channel7->CCR |= (3 << 12); //Prioridade maxima pra esse canal

	//Configurando canal TX DMA
	DMA1_Channel6->CPAR = (uint32_t)(&(I2C1->DR));
	DMA1_Channel6->CMAR = (uint32_t)tx_buffer;
	DMA1_Channel6->CNDTR = 3;
	DMA1_Channel6->CCR |= (1 << 4); //O buffer vai ser escrito no periferico
	DMA1_Channel6->CCR |= (1<<7);
	DMA1_Channel6->CCR |= (3 << 12);


	aht10_Sync();
	aht10_getValue();

	while(1){
		delay_us(1000000);
		aht10_getValue();
	}

}

void aht10_Write(uint8_t* buffer, uint8_t size){
    uint32_t counter = 0;
    volatile uint32_t temp;

    for(uint8_t i = 0; i < size; i++){
        tx_buffer[i] = buffer[i];
    }

    DMA1_Channel6->CCR &= ~(1 << 0); // Garante que está desabilitado
    DMA1_Channel6->CNDTR = size;

    // Habilita o DMA no I2C1
    I2C1->CR2 |= I2C_CR2_DMAEN;

    // Inicia comunicação I2C
    I2C1->CR1 |= I2C_CR1_START;

    counter = 0xFFFF;
    while ((I2C1->SR1 & I2C_SR1_SB) == 0)
        if (counter-- == 0) return;

    // Envia endereço (write)
    I2C1->DR = 0b01110000;

    counter = 0xFFFF;
    while ((I2C1->SR1 & I2C_SR1_ADDR) == 0)
        if (counter-- == 0) return;

    temp = I2C1->SR2; // limpa o ADDR

    // Agora que o slave respondeu, habilita o DMA
    DMA1_Channel6->CCR |= (1 << 0);  // Habilita canal DMA

    // Aguarda fim da transferência
    counter = 0xFFFF;
    while (!(DMA1->ISR & DMA_ISR_TCIF6))
        if (counter-- == 0) return;

    DMA1_Channel6->CCR &= ~(1 << 0); // desabilita canal
    DMA1->IFCR |= DMA_IFCR_CTCIF6;   // limpa flag

    // Espera transferência final (BTF)
    while ((I2C1->SR1 & I2C_SR1_BTF) == 0);

    // Gera STOP
    I2C1->CR1 |= I2C_CR1_STOP;

    while (I2C1->CR1 & I2C_CR1_STOP); // Espera final
}

/*void aht10_Write(uint8_t* buffer, uint8_t size){
	uint32_t counter = 0;
	uint32_t temp = 0;

	for(uint8_t i = 0; i < size;i++){
		tx_buffer[i] = buffer[i];
	}

	DMA1_Channel6->CNDTR = size;
	DMA1_Channel6->CCR |= (1 << 0);

	I2C1->CR1 |= (1 << 8); //Start bit

	counter = 0xFFFF;
	while ((I2C1->SR1&0x0001) != 0x0001)																		//wait loop until SB: EV5 is set
			if (counter-- == 0)
				return;

	I2C1->DR = 0b01110000; // AHT10_AD << 1 | WRITE

	counter = 0xFFFF;
	while ((I2C1->SR2&0x0002) != 0x0002)																		//wait loop until SB: EV5 is set
			if (counter-- == 0)
				return;

	temp = I2C1->SR2;

	while(!(DMA1->ISR & DMA_ISR_TCIF6)); //Espera transferencia ser concluida
	DMA1_Channel6->CCR &= ~(1 << 0); //desabilita canal
	DMA1->IFCR |= (1 << 21); //Limpa a flag

	//EV8_2: Wait until BTF is set before programming STOP.
	while ((I2C1->SR1 & 0x00004) != 0x000004);
	I2C1->CR1 |= (1 << 9); //Stop bit

	//Make sure the STOP bit is cleared by hardware before write access CR1
	while ((I2C1->CR1 & 0x200) == 0x200);
}*/

/*void aht10_Read(uint8_t* buffer, uint8_t size){
	uint32_t counter = 0;
	uint32_t temp = 0;

	DMA1_Channel7->CNDTR = size;
	DMA1_Channel7->CCR |= (1 << 0);

	I2C1->CR1 |= (1 << 8); //Start bit

	counter = 0xFFFF;
	while ((I2C1->SR1&0x0001) != 0x0001)																		//wait loop until SB: EV5 is set
			if (counter-- == 0)
				return;

	I2C1->DR = 0b01110001; // AHT10_AD << 1 | read

	counter = 0xFFFF;
	while ((I2C1->SR2&0x0002) != 0x0002)																		//wait loop until SB: EV5 is set
			if (counter-- == 0)
				return;

	temp = I2C1->SR2;

	while(!(DMA1->ISR & DMA_ISR_TCIF7)); //Espera transferencia ser concluida
	DMA1_Channel7->CCR &= ~(1 << 0); //desabilita canal
	DMA1->IFCR |= (1 << 21); //Limpa a flag

	//EV8_2: Wait until BTF is set before programming STOP.
	while ((I2C1->SR1 & 0x00004) != 0x000004);
	I2C1->CR1 |= (1 << 9); //Stop bit

	//Make sure the STOP bit is cleared by hardware before write access CR1
	while ((I2C1->CR1 & 0x200) == 0x200);

	for(uint8_t i = 0; i<size; i++){
		buffer[i] = rx_buffer[i];
	}


}*/
void aht10_Read(uint8_t* buffer, uint8_t size){
    uint32_t counter = 0;
    volatile uint32_t temp;

    // Desabilita canal e configura o tamanho
    DMA1_Channel7->CCR &= ~DMA_CCR_EN;
    DMA1_Channel7->CNDTR = size;

    // Habilita DMA no I2C
    I2C1->CR2 |= I2C_CR2_DMAEN;

    // Marca última transferência com NACK no final (requerido em recepção DMA)
    I2C1->CR2 |= I2C_CR2_LAST;

    // Gera START
    I2C1->CR1 |= I2C_CR1_START;

    counter = 0xFFFF;
    while ((I2C1->SR1 & I2C_SR1_SB) == 0)
        if (--counter == 0) return;

    // Envia endereço com bit de leitura
    I2C1->DR = 0b01110001;

    counter = 0xFFFF;
    while ((I2C1->SR1 & I2C_SR1_ADDR) == 0)
        if (--counter == 0) return;

    temp = I2C1->SR2; // limpa o ADDR

    // Agora que o escravo respondeu, ativa o canal DMA
    DMA1_Channel7->CCR |= DMA_CCR_EN;

    // Espera a recepção via DMA concluir
    counter = 0xFFFF;
    while (!(DMA1->ISR & DMA_ISR_TCIF7)) {
        if (--counter == 0) return;
    }

    // Desativa canal e limpa flag
    DMA1_Channel7->CCR &= ~DMA_CCR_EN;
    DMA1->IFCR |= DMA_IFCR_CTCIF7;

    // Aguarda BTF antes do STOP
    //while ((I2C1->SR1 & I2C_SR1_BTF) == 0);

    I2C1->CR1 |= I2C_CR1_STOP;
    while (I2C1->CR1 & I2C_CR1_STOP);

    // Copia dados lidos para o buffer de destino
    for(uint8_t i = 0; i < size; i++){
        buffer[i] = rx_buffer[i];
    }
}


void aht10_getValue(void){
	uint8_t buffer[6];
	buffer[0] = AHT10_TRIG;
	buffer[1] = 0x33;
	buffer[2] = 0x00;

	aht10_Write(buffer, 3);
	delay_us(75000);

	aht10_Read(buffer, 6);

	uint32_t umidade = ( ((uint32_t)buffer[1] << 16) | ((uint32_t)buffer[2] << 8) | (buffer[3]) ) >> 4 ;
	uint32_t temperatura = ((uint32_t)(buffer[3]&0x0F) << 16) | ((uint32_t)buffer[4] << 8) | (uint32_t)buffer[5] ;

	umid = ((float)umidade / 1048576.0f) * 100.0f;
	temp = ((float)temperatura / 1048576.0f) * 200.0f - 50.0f;

	i_temp = (int)(temp * 100.0f + 0.5f);
	i_umid = (int)(umid * 100.0f + 0.5f);

}

void aht10_Sync(void){
	uint8_t temp = AHT10_RESET;
	aht10_Write(&temp,1);
	delay_us(20000);
}

void delay_us(uint32_t us) {
    // Cada iteração do loop leva aproximadamente 8 ciclos
    // Com clock de 8 MHz, temos 1 ciclo = 0,125 µs
    // Logo, 1 iteração ≈ 1 µs (aproximado)
    for(uint32_t i = 0; i < us; i++) {
        __asm volatile("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
    }
}