/*
 * adc.h
 *
 *  Created on: Jul 29, 2025
 *      Author: jan
 */

#ifndef ADC_H_
#define ADC_H_
#include "stm32f1xx.h"
#include <stdint.h>

void adc_init(void);
uint16_t adc_read(void);


#endif /* ADC_H_ */
