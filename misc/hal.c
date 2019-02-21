/**
===============================================================================
 Name        : hal.c
 Author      : $(author)
 Version     : $Revision: 36 $
 Url         : $URL: https://repo1.mydevil.net/svn/priv/bawolski/stm8/trunk/tig_controller/misc/hal.c $
 Copyright   : $(copyright)
 Description : hardware abstraction layer module
===============================================================================
*/

#include "stm8s.h"
#include "stm8s_irq.h"
#include "hal.h" 
#include "led.h"

volatile uint16_t Timeout;

void TIMER1_isr(void) __interrupt(IRQ_TIM1)
{
	if (Timeout > 0)
	{
		Timeout--;
	}

	TIM1->SR1 &= ~(1<<0);
}


uint8_t  InitClock(void)
{
  //CLK->CKDIVR = (3u<<3) |(1u<<1) ; // Fmaster is 16MHz / 8 and CPU clk is Fmaster / 2
  // which results in Fmaster 2MHz and CPU Clk is 1MHz
  // Fmaster 2MHz  routed to Timer 1, TImer 4, UART and SPI
  CLK->CKDIVR = (0u<<3) |(1u<<1) ; // Fmaster is 16MHz / 8 and CPU clk is Fmaster / 2
  CLK->PCKENR1 |= CLK_PCKENR1_TIM1 | \
		  	  	  CLK_PCKENR1_SPI  | \
				  CLK_PCKENR1_TIM4 | \
				  CLK_PCKENR1_TIM2 | \
				  CLK_PCKENR1_UART1;

  CLK->PCKENR2  |= CLK_PCKENR2_ADC;

  return 0;
}




inline void clear_timeout(void)
{
   disableInterrupts();
   Timeout=0;
   enableInterrupts();
}


inline void wait_for_timeout(void)
{
	while (!(Timeout==0))
	    ;
}

/**
 *
 * @param time
 */
inline void set_timeout(uint16_t time)
{
	Timeout = time;
}
