/*
 * led.h
 *
 *  Created on: Jul 11, 2018
 *      Author: marcin
 */

#ifndef MISC_LED_H_
#define MISC_LED_H_

#include "stm8s.h"
#include "stm8s_irq.h"

void led_init_lcd(void);
void led_set_digit(uint8_t d);
void led_select_active_pos(uint8_t n);

void TIMER4_isr(void) __interrupt(IRQ_TIM4);

#endif /* MISC_LED_H_ */

