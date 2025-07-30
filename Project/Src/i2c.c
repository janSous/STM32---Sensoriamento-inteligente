/*
 * i2c.c
 *
 *  Created on: Jul 29, 2025
 *      Author: jan
 */
#include "i2c.h"


void i2c_dma_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
    GPIOB->CRL |= GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7; //PB6 -> SCL, PB7->SDA

    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    I2C1->CR1 |= (1 << 15);
    I2C1->CR1 &= ~(1 << 15);
    I2C1->CR2 |= 8;
    I2C1->CCR |= 40;
    I2C1->TRISE |= 9;
    I2C1->CR1 |= I2C_CR1_PE | I2C_CR1_ACK;
    I2C1->CR2 |= (1 << 11) | (1 << 12);

    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    DMA1_Channel7->CPAR = (uint32_t)(&(I2C1->DR));
    DMA1_Channel7->CMAR = (uint32_t)rx_buffer;
    DMA1_Channel7->CCR |= DMA_CCR_MINC | DMA_CCR_PL;

    DMA1_Channel6->CPAR = (uint32_t)(&(I2C1->DR));
    DMA1_Channel6->CMAR = (uint32_t)tx_buffer;
    DMA1_Channel6->CCR |= DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_PL;
}


