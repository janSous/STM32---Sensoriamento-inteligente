/*
 * adc.c
 *
 *  Created on: Jul 29, 2025
 *      Author: jan
 */
#include "adc.h"

void adc_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    ADC1->CR2 |= ADC_CR2_ADON;
    delay_us(10);
    ADC1->CR2 |= ADC_CR2_ADON;
    delay_us(10);
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL);

    ADC1->SMPR2 |= (7 << 24);  // Canal 8 (PB0), amostragem longa
    ADC1->SQR3 = 8;            // Seleciona canal 8
}

uint16_t adc_read(void) {
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC));
    return ADC1->DR;
}


