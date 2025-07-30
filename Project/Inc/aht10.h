/*
 * aht10.h
 *
 *  Created on: Jul 29, 2025
 *      Author: jan
 */

#ifndef AHT10_H_
#define AHT10_H_
#include <stdint.h>
#include "stm32f1xx.h"
#include "nrf24.h"

extern uint8_t tx_buffer[3];
extern uint8_t rx_buffer[6];

// Comandos AHT10
#define AHT10_TRIG   0xAC
#define AHT10_RESET  0xBA

void aht10_Sync(void);
void aht10_Write(uint8_t*, uint8_t);
void aht10_Read(uint8_t*, uint8_t);
void delay_us(uint32_t us);
#endif /* AHT10_H_ */
