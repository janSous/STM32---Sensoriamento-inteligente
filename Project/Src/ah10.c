#include "aht10.h"

//Arquio.c não finalizado, apenas para testes

//Tasks: Implementar delay 

OPERATION_STATE aht_Sync(void){
    i2c_start();
    I2C1->DR = AHT_ADR_WRITE;
    while(!(I2C1->SR1 & (1<<1)));
    uint8_t temp = I2C1->SR1| I2C1->SR2; //limpa o bit ADR
    
    return AHT10_OK;
}

OPERATION_STATE aht_Write(uint8_t cmd) {
    if(!aht_Sync()){
        return AHT10_ERR;
    }
    
    while(!(I2C1->SR1 & (1<<7))); //Espera o buffer ficar livre
    I2C1->DR = cmd; 
    
    while(!(I2C1->SR1 & (1<<7)));
    I2C1->DR = 0x33;
   
    while(!(I2C1->SR1 & (1<<7)));
    I2C1->DR = 0;

    while(!(I2C1->SR1 & (1<<2))); // espera o ultimo byte ser tranferido
        
    i2c_stop();
    return AHT10_OK;
}
OPERATION_STATE aht_Read(uint8_t* buffer, uint8_t size) {
    if(!aht_Sync()){
        return AHT10_ERR;
    }

    //varaiavel auxiliar para identificar a leitura do penultimo byte
    uint8_t resto = size;

    for(;resto > 2; resto--){
        while(!(I2C1->SR1 & (1<<6)));
        buffer[size - resto] = I2C1->CR;
        I2C1->CR1 |= (1 << 10); //ACK = 1
    }

    //Lê penultimo byte
    while(!(I2C1->SR1 & (1<<6)));
    buffer[size - resto] = I2C1->CR;
    I2C1->CR1 &= ~(1 << 10); //ACK = 0

    i2c_stop();

    //Lê o ultimo byte e encerra a comunicação
    while(!(I2C1->SR1 & (1<<6)));
    buffer[size - resto] = I2C1->CR;
    return AHT10_OK; 
}

float getTemperature(uint8_t* buffer) {
    
    float temp = 0;
    if(!(buffer[0] & 0x40)){ //verifica se os valores estao calibrados
        aht_Write(AHT_CMD_INIT); //manda comando para calibrar o sensor
    }

    uint32_t raw_data = (((uint32_t)(buffer[3] & 0xF) << 16)|((uint32_t)buffer[4] << 8)| buffer[5]);
    temp = (float)(raw_data * 200.00 / 1048576.00) - 50.00; //valor convertido
    
    return temp;
}

float getHumidity(uint8_t* buffer) {
    float hum = 0;
   if(!(buffer[0] & 0x40)){ //verifica se os valores estao calibrados
        aht_Write(AHT_CMD_INIT); //manda comando para calibrar o sensor
    }

    uint32_t raw_data = (((uint32_t)(buffer[3] >> 4))|((uint32_t)buffer[4] << 4)| (buffer[5] << 12));
    hum = (float)(raw_data*100.00/1048576.00);//valor convertido
    
    return hum;
}
