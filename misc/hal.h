/**
===============================================================================
 Name        : hal.h
 Author      : $(author)
 Version     : $Revision: 36 $
 Url         : $URL: https://repo1.mydevil.net/svn/priv/bawolski/stm8/trunk/tig_controller/misc/hal.h $
 Copyright   : $(copyright)
 Description : hardware abstraction layer header
===============================================================================
*/


#include "stm8s.h"
#include "../include/stm8s_irq.h"

#ifndef _HAL_H_
#define _HAL_H_


#define ENC_SWITCH (1<<6)
#define ENC_SWITCH_PORT GPIOD
#define timeoutElapsed (Timeout==0)

#define LED_PORT  (GPIOB)
#define RED_LED   (1<<5)


inline void clear_timeout(void);
uint8_t  InitClock(void);
void SetupTimer1(uint16_t period );
void setup_timer4(uint8_t period );
uint8_t keyOn(void);
inline void wait_for_timeout(void);
inline void set_timeout(uint16_t time);
void setup_timer4(uint8_t period );
void TIMER1_isr(void) __interrupt(IRQ_TIM1);

extern volatile uint16_t Timeout;

#endif  

