#include "gpio.h"
#include "stm32f103xb.h"

void gpio_init(GPIOx Port){
    switch(Port){
        case gpioa:
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
            break;
        case gpiob:
            RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
            break;
        case gpioc:
            RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
            break;
        default:
            return;
    }
}

void afio_init(void){
    RCC->APB2ENR |= (1 << 0);
}

void setup_pin(GPIOx Port, PIN p, PIN_SPEED speed, PIN_CONFIG conf){
    uint32_t config = 0;
    config = ((conf & 0x3) << 2) | (speed & 0x3);

    if(p > 7){
        switch(Port){
            case gpioa:
                GPIOA->CRH &= ~(0xF << ((p-8) * 4));
                GPIOA->CRH |= (config << ((p-8) * 4));
                
                break;
            case gpiob:
                GPIOB->CRH &= ~(0xF << ((p-8) * 4));
                GPIOB->CRH |= (config << ((p-8) * 4));
                break;
            case gpioc:
                GPIOC->CRH &= ~(0xF << ((p-8) * 4));
                GPIOC->CRH |= (config << ((p-8) * 4));
                break;
            default:
                return;
            }
    }
    else{
        switch(Port){
            case gpioa:
                GPIOA->CRL &= ~(0xF << (p*4));
                GPIOA->CRL |= (config << (p*4));
                break;
            case gpiob:
                GPIOB->CRL &= ~(0xF << (p*4));
                GPIOB->CRL |= (config << (p*4));
                break;
            case gpioc:
                GPIOC->CRL &= ~(0xF << (p* 4));
                GPIOC->CRL |= (config << (p*4));
                break;
            default:
                return;
            }
    }
}
    
PIN_STATE read_pin(GPIOx port, PIN p){
    switch(port){
        case gpioa:
            if(GPIOA->IDR & (1<<p)){
                return ON;
            }
            else{
                return OFF;
            }
        case gpiob:
            if(GPIOB->IDR & (1<<p)){
                return ON;
            }
            else{
                return OFF;
            }
        case gpioc:
            if(GPIOC->IDR & (1<<p)){
                return ON;
            }
            else{
                return OFF;
            }
        default:
            return OFF;
    }
}

void setReset_pin(GPIOx port, PIN p, PIN_STATE level){
    if(level){
        switch (port)
        {
        case gpioa:
            GPIOA->ODR |= (1 << p);
            break;
        case gpiob:
            GPIOB->ODR |= (1 << p);
            break;
        case gpioc:
            GPIOC->ODR |= (1 << p);
            break;
        default:
            break;
        }
    }
    else{
        switch (port)
        {
        case gpioa:
            GPIOA->ODR &= ~(1 << p);
            break;
        case gpiob:
            GPIOB->ODR &= (~1 << p);
            break;
        case gpioc:
            GPIOC->ODR &= ~(1 << p);
            break;
        default:
            break;
        }
    }
}