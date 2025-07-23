#include "i2c.h"


void i2c_clock_init(uint8_t channelI2C){
    if(channelI2C){
        RCC->APB1ENR |= (1 << 22);
    }
    else if(!channelI2C){
        RCC->APB1ENR |= (1 << 21);
    }
}

void i2c_init(uint8_t i2c_freq, DEVICE_I2C dev){

    //Reseta o periferico
    I2C1->CR1 |= (1 << 15); 
    I2C1->CR1 &= ~(1 << 15);
    
    if(i2c_freq < I2C_MIN_FREQ){
        i2c_freq = I2C_MIN_FREQ;
    }

    if(i2c_freq > I2C_MAX_FREQ){
        i2c_freq = I2C_MAX_FREQ;
    }

    I2C1->CR2 |= (i2c_freq & 0x3F); //garante que so seja escrito 6bits

    //configura ccr e trise de acordo com as caracteristicas do dispositivo
    switch (dev)
    {
    case AHT10:
        if(i2c_freq == 8) {
            I2C1->CCR |= 40; //Frequencia do SCL (100KHz)
            I2C1->TRISE |= 9; //Tempo maximo de subida 
        }
        break;
    default:
        return;
    }
    
    I2C1->CR1 |= (1 << 0); //habilita o periferico
}

void i2c_int_enable(I2C_INT intType){
    switch (intType)
    {
    case DMA_EN:
        I2C1->CR2 |= (1 << 11);
        break;
    case BUFFER_INT:
        I2C1->CR2 |= (1 << 10);
        break;
    case EVENT_INT:
        I2C1->CR2 |= (1 << 9);
        break;
    case ERROR_INT:
        I2C1>CR2 |= (1 << 8);
        break;
    default:
        break;
    }
}

void i2c_start(void){
    I2C1->CR1 |= (1 << 10) | (1 << 8); //Habilita o ACK e o START
    while(!(I2C1->SR1 & 0x1)){
        //espera ate a transição do clock acontecer
    }
}

void i2c_stop(void){
    I2C1->CR1 |= (1 << 9); //Habilita o bit STOP
}