#ifndef GPIO_LIB
#define GPIO_LIB
#include <stdint.h>
#include <stdbool.h>

typedef enum{
    gpioa = 0,
    gpiob,
    gpioc
}GPIOx;

typedef enum{
    P0 = 0, P1, P2, P3, P4, P5 , P6, P7 ,P8, P9 , P10,
    P11, P12, P13, P14,P15
}PIN;

typedef enum{
    IN = 0,
    OUT_10MHZ,
    OUT_2MHZ,
    OUT_50MHZ
}PIN_SPEED;

typedef enum{
    CNF_0 = 0, // se IN, cnf_0 = analog, senao cnf_0 = general output push pull
    CNF_1,     // se IN, cnf_1 = floating input, senao cnf_1 = general out open-drain
    CNF_2,       //se IN cnf_2 = in pullup/pulldown, senao cnf_2 = afio push pull
    CNF_3     // out, cnf_3 = out open drain
}PIN_CONFIG;

typedef enum{
    OFF = 0,
    ON
}PIN_STATE;

typedef enum{
    GPIO_ERR = 0,
    GPIO_OK
}GPIO_OPERATION;

void gpio_init(GPIOx);
void afio_init(void);
void setup_pin(GPIOx, PIN, PIN_SPEED, PIN_CONFIG);
PIN_STATE read_pin(GPIOx, PIN);
void setReset_pin(GPIOx, PIN, PIN_STATE);

#endif