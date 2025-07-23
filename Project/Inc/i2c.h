#ifndef I2C_LIB
#define I2C_LIB
#include <stdint.h>


#define I2C_1   0
#define I2C_2   1

#define I2C_MIN_FREQ   (0x2) //frequencia minima
#define I2C_MAX_FREQ  (I2C_MIN_FREQ + 48) //Frequencia maxima

typedef enum{
    AHT10 = 0
}DEVICE_I2C;

typedef enum{
    DMA_EN = 0,
    BUFFER_INT,
    EVENT_INT,
    ERROR_INT
}I2C_INT;

void i2c_clock_init(uint8_t);
void i2c_init(uint8_t,DEVICE_I2C);
void i2c_int_enable(I2C_INT);
void i2c_start(void);
void i2c_stop(void);
#endif