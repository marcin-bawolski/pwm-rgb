/*
 * led.c
 *
 *  Created on: Jul 11, 2018
 *      Author: marcin
 */

#include "led.h"
#include "stm8s.h"

#define LCD_TICK (5ul*(2000000ul/64ul)/(1000ul) )

#define LED_SEG_PIN_A   (1u<<4)
#define LED_SEG_PIN_B   (1u<<5)
#define LED_SEG_PIN_C   (1u<<2)
#define LED_SEG_PIN_D   (1u<<3)
#define LED_SEG_PIN_E   (1u<<4)
#define LED_SEG_PIN_F   (1u<<6)
#define LED_SEG_PIN_G   (1u<<7)
#define LED_SEG_PIN_H

#define LED_SEG_AB_PORT    GPIOB
#define LED_SEG_CDE_PORT   GPIOD
#define LED_SEG_FG_PORT    GPIOC

#define LED_SEG_H_PORT

#define LED_PORT_ANODES GPIOC
#define LED_PORT_AN2 GPIOC
#define LED_PORT_AN3 GPIOC

#define LED_PIN_AN1  (1u<<0)
#define LED_PIN_AN2  (1u<<1)
#define LED_PIN_AN3  (1u<<2)

#define LED_PIN_ANNODES_SHIFT 3

#define LED_PIN_ANNODES_MASK  ((LED_PIN_AN1|LED_PIN_AN2|LED_PIN_AN3)<<LED_PIN_ANNODES_SHIFT)


#define SEG_A  (1u<<0)
#define SEG_B  (1u<<1)

#define SEG_C  (1u<<2)
#define SEG_D  (1u<<3)
#define SEG_E  (1u<<4)

#define SEG_F  (1u<<5)
#define SEG_G  (1u<<6)
#define SEG_H  (1u<<7)

#define SEG_A_HW  (1u<<4)
#define SEG_B_HW  (1u<<5)

#define SEG_C_HW  (1u<<2)
#define SEG_D_HW  (1u<<3)
#define SEG_E_HW  (1u<<4)

#define SEG_F_HW  (1u<<6)
#define SEG_G_HW  (1u<<7)


#define SEG_AB  (SEG_A|SEG_B)
#define SEG_CDE (SEG_C|SEG_D|SEG_E)
#define SEG_FG  (SEG_F|SEG_G)

#define SEG_AB_HW  (SEG_A_HW|SEG_B_HW)
#define SEG_CDE_HW (SEG_C_HW|SEG_D_HW|SEG_E_HW)
#define SEG_FG_HW  (SEG_F_HW|SEG_G_HW)


#define SEG_AB_HW_shift  4
#define SEG_CDE_HW_shift 2
#define SEG_FG_HW_shift  6

#define SEG_AB_MSK  (SEG_AB<<0)
#define SEG_CDE_MSK (SEG_CDE<<0)
#define SEG_FG_MSK  (SEG_FG<<0)


static void led_init_segment_pins(void);
static void led_init_anodes_pins(void);

volatile uint8_t led_cnt[3]={0,0,0};

#warning "Marcin Optimize so no invertion at run-time is needed"
static const uint8_t digits[11] = {
/* 0 */		(SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F        ),
/* 1 */		(        SEG_B + SEG_C                                ),
/* 2 */		(SEG_A + SEG_B +         SEG_D + SEG_E +         SEG_G),
/* 3 */		(SEG_A + SEG_B + SEG_C + SEG_D                 + SEG_G),
/* 4 */		(        SEG_B + SEG_C +                 SEG_F + SEG_G),
/* 5 */		(SEG_A +         SEG_C + SEG_D +         SEG_F + SEG_G),
/* 6 */		(SEG_A +         SEG_C + SEG_D + SEG_E + SEG_F + SEG_G),
/* 7 */		(SEG_A + SEG_B + SEG_C                                ),
/* 8 */		(SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G),
/* 9 */		(SEG_A + SEG_B + SEG_C + SEG_D +         SEG_F + SEG_G),
/* blank */	(                                                    0)
};

void TIMER4_isr(void) __interrupt(IRQ_TIM4)
{
	static uint8_t multiplex_pos=0;

	uint8_t digit = led_cnt[multiplex_pos];
	led_set_digit(10);
	led_select_active_pos(multiplex_pos);
	led_set_digit(digit);

	multiplex_pos++;

	if (multiplex_pos > 2)
	{
		multiplex_pos=0;
	}

	TIM4->SR1 &= ~(1ul<<0);
}


void led_set_digit(uint8_t d)
{
	uint8_t segments;
	uint8_t AB,CDE,FG;
	if (d>10)
	{
		d=10;
	}
	segments= digits[d];

	AB =  (((segments & SEG_AB )>>0) << SEG_AB_HW_shift);
	CDE = (((segments & SEG_CDE)>>2) << SEG_CDE_HW_shift);
	FG =  (((segments & SEG_FG )>>5) << SEG_FG_HW_shift);

	LED_SEG_AB_PORT->ODR  = ((LED_SEG_AB_PORT->ODR  | SEG_AB_HW))  & ~AB;
	LED_SEG_CDE_PORT->ODR = ((LED_SEG_CDE_PORT->ODR | SEG_CDE_HW)) & ~CDE;

	// take care of ANODES (3,4,5 HW pos)
	LED_SEG_FG_PORT->ODR  = ((LED_SEG_FG_PORT->ODR | SEG_FG_HW) & ~FG);
}

void led_select_active_pos(uint8_t n)
{
	GPIOC->ODR |= LED_PIN_ANNODES_MASK;
	GPIOC->ODR &= ~(1u << (n + LED_PIN_ANNODES_SHIFT));
}




void led_setup_timer4(uint8_t period )
{

  // Assuming FMaster = 2 MHz and prescaler = 64, have 1us clock in timer
  // Timer Auto-reload preload enable, UpCounting, Timer Enabled
  TIM4->CR1 = (1u<<7) | (1u<<0);
  TIM4->ARR =  period;
  TIM4->PSCR = 0x06;

  TIM4->IER = 1u<<0;     /*!< interrupt enable register*/
}



void led_init_lcd(void)
{
	led_init_anodes_pins();
	led_init_segment_pins();

	led_setup_timer4(LCD_TICK);
}



static void led_init_segment_pins(void)
{
	LED_SEG_AB_PORT->DDR  |= SEG_AB_HW;
	LED_SEG_CDE_PORT->DDR |= SEG_CDE_HW;
	LED_SEG_FG_PORT->DDR  |= SEG_FG_HW;

	LED_SEG_AB_PORT->ODR  |= SEG_AB_HW;
	LED_SEG_CDE_PORT->ODR |= SEG_CDE_HW;
	LED_SEG_FG_PORT->ODR  |= SEG_FG_HW;
}

static void led_init_anodes_pins(void)
{
	LED_PORT_ANODES->DDR |= LED_PIN_ANNODES_MASK;
	LED_PORT_ANODES->ODR |= LED_PIN_ANNODES_MASK;
}


