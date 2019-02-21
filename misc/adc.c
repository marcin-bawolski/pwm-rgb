/*
 * adc.c
 *
 *  Created on: Dec 1, 2017
 *      Author: marcin
 */


#include "../stm8s.h"


void adc_init(void)
{
	// use ADin2 on Pin C4
	CLK->PCKENR1 |= (1<<3) | CLK_PCKENR1_TIM1;
	CLK->PCKENR2 |= CLK_PCKENR2_ADC;

	ADC1->CR1 =1;

	ADC1->CSR |= 2;
	ADC1->CR1 &= ~(1<<1); // single conversion


	GPIOC->DDR &= ~(1<<4);
}


uint16_t get_adc(void)
{
	uint16_t adc;

	ADC1->CSR &= ~(1<<7); //  clear EOC
	ADC1->CR1 |= 1;

	while( !(ADC1->CSR & 1<<7))
		;

	ADC1->CSR &= ~(1<<7); //  clear EOC

	adc = (uint16_t)(ADC1->DRH);

	return adc;
}
