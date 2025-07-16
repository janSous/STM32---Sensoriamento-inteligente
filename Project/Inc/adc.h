#ifndef ADC_LIB
#define ADC_LIB

#include <stdint.h>

#define ADC_RES_8BITS
#define ADC_RES_10_BITS
#define ADC_RES_12_BITS

typedef enum{
    adc1 = 0,
    adc2
}ADCx;

typedef enum{
    RIGHT_AL = 0,
    LEFT_AL
}ALIGNED_MODE;

typedef enum{
    EOC = 0,
    JEOC,   
    AWD     //analog watchdog
}ADC_INTERRUPT;

typedef enum{
    ADC_CH0 = 0, ADC_CH1, ADC_CH2, ADC_CH3, ADC_CH4, ADC_CH5,
    ADC_CH6, ADC_CH7, ADC_CH8, ADC_CH9, ADC_CH10,ADCH_CH11,
    ADC_CH12, ADC_CH13, ADC_CH14, ADC_CH15
}ADC_EXT_CHANNEL;

typedef enum{
    SINGLE = 0,
    CONTINUOUS,
    SCAN,
    DESCONTINOUS
}ADC_MODE;


//Prototipos
void adc_clock_init(ADCx);
void adc_setup(ADCx,ADC_MODE,uint8_t);
void adc_start(ADCx);
uint16_t adc_read(void);
void adc_int_enable(ADC_EXT_CHANNEL);

//funções customizadas para componentes
void adc_ldr_init(ADCx, ADC_MODE);

#endif