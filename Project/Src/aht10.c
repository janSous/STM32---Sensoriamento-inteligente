/*
 * aht10.c
 *
 *  Created on: Jul 29, 2025
 *      Author: jan
 */
#include "aht10.h"

//buffers usados na dma
uint8_t tx_buffer[3];
uint8_t rx_buffer[6];

void aht10_Write(uint8_t* buffer, uint8_t size) {
    for(uint8_t i = 0; i < size; i++)
        tx_buffer[i] = buffer[i];

    DMA1_Channel6->CCR &= ~DMA_CCR_EN;
    DMA1_Channel6->CNDTR = size;

    I2C1->CR2 |= I2C_CR2_DMAEN;
    I2C1->CR1 |= I2C_CR1_START;

    while (!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->DR = 0x70; //comando de escrita

    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;

    DMA1_Channel6->CCR |= DMA_CCR_EN;

    while (!(DMA1->ISR & DMA_ISR_TCIF6));
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;
    DMA1->IFCR |= DMA_IFCR_CTCIF6;

    while (!(I2C1->SR1 & I2C_SR1_BTF));
    I2C1->CR1 |= I2C_CR1_STOP;
    while (I2C1->CR1 & I2C_CR1_STOP);
}

void aht10_Read(uint8_t* buffer, uint8_t size) {
    DMA1_Channel7->CCR &= ~DMA_CCR_EN;
    DMA1_Channel7->CNDTR = size;

    I2C1->CR2 |= I2C_CR2_DMAEN | I2C_CR2_LAST;
    I2C1->CR1 |= I2C_CR1_START;

    while (!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->DR = 0x71; //comando de leitura

    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2;

    DMA1_Channel7->CCR |= DMA_CCR_EN;

    while (!(DMA1->ISR & DMA_ISR_TCIF7));
    DMA1_Channel7->CCR &= ~DMA_CCR_EN;
    DMA1->IFCR |= DMA_IFCR_CTCIF7;

    I2C1->CR1 |= I2C_CR1_STOP;
    while (I2C1->CR1 & I2C_CR1_STOP);

    for (uint8_t i = 0; i < size; i++)
        buffer[i] = rx_buffer[i];
}

void delay_us(uint32_t us) {
    for (uint32_t i = 0; i < us; i++) {
        __asm volatile("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n");
    }
}

//Estabele conexão com o sensor
void aht10_Sync(void) {
    uint8_t cmd = AHT10_RESET;
    aht10_Write(&cmd, 1);
    delay_us(20000);
}
