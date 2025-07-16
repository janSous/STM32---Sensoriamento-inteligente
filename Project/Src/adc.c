#include "adc.h"

void adc_clock_init(ADCx per){
    if(per){
        RCC->APB2ENR |= (1 << 10); 
    }
    else{
        RCC->APB2ENR |= (1 << 9);
    }
}

void adc_setup(ADCx per,ADC_MODE md,uint8_t n_channel){
    if(per){
        switch(md){
            case SINGLE:
                ADC2->CR2 &= ~(1<<1);
                break;
            case CONTINUOUS:
                ADC2->CR2 |= (1 << 1);
                break;
            case DESCONTINOUS:
                if(!(ADC2->CR2 & 0X2)){ // Modo continuo desabilitado
                    ADC2->CR1 &= ~(1 << 11);
                    ADC2->CR1 |= (1 << 11);
                }
                break;
            case SCAN:
                ADC2->CR1 &= ~(1 << 8);
                ADC2->CR1 |= (1 << 8);
                break;
        }

        ADC2->SQR1 &= ~(0xF << 20);
        if(n_channel > 16){
            ADC2->SQR1 |= (0xf << 20); //força a leitura de 16 canais
        }
        else{
            ADC2->SQR1 |= ((n_channel - 1) << 20);
        }
    }

    else{
        switch(md){
            case SINGLE:
                ADC1->CR2 &= ~(1<<1);
                break;
            case CONTINUOUS:
                ADC1->CR2 &= ~(1<<1);
                ADC1->CR2 |= (1 << 1);
                break;
            case DESCONTINOUS:
                ADC1->CR1 &= ~(1 << 11);
                ADC1->CR1 |= (1 << 11);
                break;
            case SCAN:
                ADC1->CR1 &= ~(1 << 8);
                ADC1->CR1 |= (1 << 8);
                break;
        }

        ADC1->SQR1 &= ~(0xF << 20);
        if(n_channel > 16){
            ADC1->SQR1 |= (0xf << 20); //força a leitura de 16 canais
        }
        else{
            ADC1->SQR1 |= ((n_channel - 1) << 20);
        }
    }
}

void adc_start(ADCx per, uint16_t* value){
    if(!per){
        ADC1->CR2 |= (1<<0); //ativa adc
        for (volatile int i = 0; i < 1000; i++); //espera
        ADC1->CR2 |= (1<<2); //ativa calibração

        while(ADC1->CR2 & 0x4){
            //espera calibração
        }

        ADC1->CR2 |= (1<<0);

        ADC1->CR2 &= ~(7U << 17);
        ADC1->CR2 |= (7U << 17); //Seleciona swstart como trigger da conversão
        ADC1->CR2 |= (1 << 20); //EXTRIG = 1
        ADC1->CR2 |= (1 << 22); //Começa a conversão

        while(!(ADC1->SR & 0x2)){
            //Espera a conversão terminar
        }

        *value = ADC1->DR; //Guarda dado e limpa o bit EOC
    }
    else{
        ADC2->CR2 |= (1<<0); //ativa adc
        for (volatile int i = 0; i < 1000; i++); //espera
        ADC2->CR2 |= (1<<2); //ativa calibração

        while(ADC2->CR2 & 0x4){
            //espera calibração
        }

        ADC2->CR2 |= (1<<0); 
        ADC2->CR2 |= (7U << 17); //Seleciona swstart como trigger da conversão
        ADC2->CR2 |= (1 << 20); //EXTRIG = 1
        ADC2->CR2 |= (1 << 22); //Começa a conversão

        while(!(ADC2->SR & 0x2)){
            //Espera a conversão terminar
        }
        *value = ADC2->DR;
    }
}






