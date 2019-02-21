
/**
===============================================================================
 Name        : main.c
 Author      : $Author: marcin $
 Description : main definition
===============================================================================
*/

// Marcin: interrupt function must be at least declared  in the same file as main() function
// Definition (code itself) can be  the in other location
 //void TIMER1_isr(void) __interrupt(IRQ_TIM1);
// void TIMER4_isr(void) __interrupt(IRQ_TIM4);
// above functions are in corresponding headers and MUST be included in main.c

#include "STM8S_StdPeriph_Driver/inc/stm8s.h"
#include "misc/hal.h"

#include "include/stm8s_irq.h"
#include "STM8S_StdPeriph_Driver/inc/stm8s_adc1.h"
#include "STM8S_StdPeriph_Driver/inc/stm8s_tim2.h"

#define TIM1_PERIOD_TICKS 5000ul


static inline void init_timer_1(void);
static inline void init_timer_2(void);
static inline void adc_init(void);
static uint16_t get_adc(uint8_t channel);


uint16_t adc;
uint32_t optimized_pwm=0;
uint16_t inverted_pwm=0;
uint8_t ch=0;

void main(void)
{
  InitClock();
  disableInterrupts();

  LED_PORT->DDR = RED_LED;
  LED_PORT->CR1 = RED_LED;

  LED_PORT->ODR &= ~(RED_LED);

  GPIOD->DDR &= (uint8_t)(~(1u<<2));
  GPIOD->DDR &= (uint8_t)(~(1u<<3));
  GPIOD->DDR &= (uint8_t)(~(1u<<5)); // AIN_5

  adc_init();
  init_timer_1();
  init_timer_2();

  enableInterrupts();
  adc =0;

    while (1)
    {
        if (++ch > 2)
        {
            ch =0;
            LED_PORT->ODR = LED_PORT->ODR ^ ((uint8_t)(RED_LED));
        }
        set_timeout(33);
        wait_for_timeout();

        switch (ch)
        {
        case 0: {
            adc = get_adc(ADC1_CHANNEL_3);
            break;
        }

        case 1: {
            adc = get_adc(ADC1_CHANNEL_4);
            break;
        }

        case 2: {
            adc = get_adc(ADC1_CHANNEL_5);
            break;
        }
        default:
            break;
        }

        if (adc > 1024)
        {
            while (1)
            {
                set_timeout(5000);
                wait_for_timeout();
                LED_PORT->ODR = LED_PORT->ODR ^ ((uint8_t)(RED_LED));
            }
        }

        optimized_pwm = (((uint32_t)(adc) * (uint32_t)TIM1_PERIOD_TICKS) / 1024ul);

        switch (ch)
        {
        case 0: {
            TIM1->CCR3H = (uint8_t)(optimized_pwm >> 8);
            TIM1->CCR3L = (uint8_t)(optimized_pwm);
            break;
        }

        case 1: {
            TIM1->CCR4H = (uint8_t)(optimized_pwm >> 8);
            TIM1->CCR4L = (uint8_t)(optimized_pwm);
            break;
        }
            break;
        case 2: {
            TIM2_SetCompare1(optimized_pwm);

            break;
        }
        default:
            break;

        }

    }

}


static inline void init_timer_1(void)
{

    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_CEN);
    TIM1->SMCR &= (uint8_t)(~TIM1_SMCR_SMS);

    TIM1->PSCRH = (uint8_t)(0 >> 8);
    TIM1->PSCRL = (uint8_t)(0);
    TIM1->EGR = (uint8_t)TIM1_PSCRELOADMODE_UPDATE;
    /* Set the Autoreload value */
    TIM1->ARRH = (uint8_t)(TIM1_PERIOD_TICKS >> 8);
    TIM1->ARRL = (uint8_t)(TIM1_PERIOD_TICKS);
    /* Set the Prescaler value */
    TIM1->PSCRH = (uint8_t)(0 >> 8);
    TIM1->PSCRL = (uint8_t)(0);
    /* Select the Counter Mode */
    TIM1->CR1 = (uint8_t)((uint8_t)(TIM1->CR1 & (uint8_t)(~(TIM1_CR1_CMS | TIM1_CR1_DIR))) | (uint8_t)(TIM1_COUNTERMODE_UP));
    /* Set the Repetition Counter value */
    TIM1->RCR = 0;

    /* Disable the Channel 1: Reset the CCE Bit, Set the Output State ,
     the Output N State, the Output Polarity & the Output N Polarity*/
    TIM1->CCER2 &= (uint8_t)(~( TIM1_CCER2_CC3E | TIM1_CCER2_CC3NE |
    TIM1_CCER2_CC3P | TIM1_CCER2_CC3NP));
    /* Set the Output State & Set the Output N State & Set the Output Polarity &
     Set the Output N Polarity */
    TIM1->CCER2 |= (uint8_t)((uint8_t)((uint8_t)(TIM1_OUTPUTSTATE_ENABLE & TIM1_CCER2_CC3E)
                                        | (uint8_t)(TIM1_OUTPUTNSTATE_DISABLE & TIM1_CCER2_CC3NE))
                                        | (uint8_t)((uint8_t)(TIM1_OCPOLARITY_HIGH & TIM1_CCER2_CC3P)
                                        | (uint8_t)(TIM1_OCPOLARITY_HIGH & TIM1_CCER2_CC3NP)));

    /* Reset the Output Compare Bits & Set the Output Compare Mode */
    TIM1->CCMR3 = (uint8_t)((uint8_t)(TIM1->CCMR3 & (uint8_t)(~TIM1_CCMR_OCM)) | (uint8_t)TIM1_OCMODE_PWM2);

    /* Reset the Output Idle state & the Output N Idle state bits */
    TIM1->OISR &= (uint8_t)(~(TIM1_OISR_OIS3 | TIM1_OISR_OIS3N));
    /* Set the Output Idle state & the Output N Idle state configuration */
    TIM1->OISR |= (uint8_t)((uint8_t)(TIM1_OISR_OIS3  & TIM1_OCIDLESTATE_RESET)
                          | (uint8_t)(TIM1_OISR_OIS3N & TIM1_OCIDLESTATE_RESET));

    /* Set the Pulse value */
    TIM1->CCR3H = (uint8_t)((TIM1_PERIOD_TICKS/2) >> 8);
    TIM1->CCR3L = (uint8_t)(TIM1_PERIOD_TICKS/2);

    /* Disable the Channel 4: Reset the CCE Bit */
    TIM1->CCER2 &= (uint8_t)(~(TIM1_CCER2_CC4E | TIM1_CCER2_CC4P));
    /* Set the Output State  &  the Output Polarity */
    TIM1->CCER2 |= (uint8_t)((uint8_t)(TIM1_OUTPUTSTATE_ENABLE & TIM1_CCER2_CC4E)
                            | (uint8_t)(TIM1_OCPOLARITY_HIGH   & TIM1_CCER2_CC4P));

    /* Reset the Output Compare Bit  and Set the Output Compare Mode */
    TIM1->CCMR4 = (uint8_t)((uint8_t)(TIM1->CCMR4 & (uint8_t)(~TIM1_CCMR_OCM)) | TIM1_OCMODE_PWM2);

    /* Set the Output Idle state */

    TIM1->OISR &= (uint8_t)(~TIM1_OISR_OIS4);

    /* Set the Pulse value */
    TIM1->CCR4H = (uint8_t)((TIM1_PERIOD_TICKS/2) >> 8);
    TIM1->CCR4L = (uint8_t)(TIM1_PERIOD_TICKS/2);

    TIM1->BKR |= TIM1_BKR_MOE;

    /* Enable the Interrupt sources */
    TIM1->IER |= (uint8_t)TIM1_IT_UPDATE;
    TIM1->CR1 |= (TIM1_CR1_CEN | TIM1_CR1_ARPE);

}





static inline void adc_init(void)
{
    // use ADin2 on Pin C4
    CLK->PCKENR1 |= (1<<3) | CLK_PCKENR1_TIM1;
    CLK->PCKENR2 |= CLK_PCKENR2_ADC;

    ADC1->CR1 &= ~ADC1_CR1_ADON;
    ADC1->CR1 |= ADC1_CR1_ADON; // Power-up

    ADC1->CR2 |= ADC1_CR2_ALIGN; // RIGHT aligned

    ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
    /* Select the ADC1 channel */
    ADC1->CSR |= (uint8_t)(ADC1_CHANNEL_3);

    ADC1->CR1 &= ~(1<<1); // single conversion

    GPIOD->DDR &= ~(1<<2); // PIN PD2 as input (A3)
    GPIOD->DDR &= ~(1<<3); // PIN PD2 as input ( A4)
    ADC1->CR1 |= ADC1_CR1_ADON;
}

static inline void init_timer_2(void)
{
    TIM2_Cmd(DISABLE);
    TIM2_TimeBaseInit(   TIM2_PRESCALER_1, TIM1_PERIOD_TICKS);
    TIM2_OC1Init(TIM2_OCMODE_PWM2, TIM2_OUTPUTSTATE_ENABLE, (TIM1_PERIOD_TICKS/3), TIM2_OCPOLARITY_HIGH);
    TIM2_OC1PreloadConfig(ENABLE);
    TIM2_SelectOCxM(TIM2_CHANNEL_1 , TIM2_OCMODE_PWM2 );
    TIM2_CCxCmd(TIM2_CHANNEL_1, ENABLE);
    TIM2_ARRPreloadConfig(ENABLE);
    TIM2_SetAutoreload(TIM1_PERIOD_TICKS);

    TIM2_Cmd(ENABLE);

}

static uint16_t get_adc(uint8_t channel)
{
    uint16_t adc;
    uint16_t templ, temph;

    do
    {
        ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
        /* Select the ADC1 channel */
        ADC1->CSR |= (uint8_t)(channel);

        ADC1->CSR &= ~(ADC1_CSR_EOC); //  clear EOC
        ADC1->CR1 |= ADC1_CR1_ADON;

        while (!(ADC1->CSR & ADC1_CSR_EOC))
            ;
        ADC1->CSR &= ~(ADC1_CSR_EOC); //  clear EOC

        /* Read LSB first */
        templ = ADC1->DRL;
        /* Then read MSB */
        temph = ADC1->DRH;

    } while (ADC1->CR3 & ADC1_CR3_OVR);

    adc = (uint16_t)((uint16_t)templ | (uint16_t)((uint16_t)temph << (uint8_t)8));

    return adc;
}

