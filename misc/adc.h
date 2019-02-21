/*
 * adc.h
 *
 *  Created on: Dec 1, 2017
 *      Author: marcin
 */

#ifndef MISC_ADC_H_
#define MISC_ADC_H_

#include "../stm8s.h"

void adc_init(void);
uint16_t get_adc(void);


#endif /* MISC_ADC_H_ */
