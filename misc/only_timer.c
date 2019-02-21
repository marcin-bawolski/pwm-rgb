/*
 * only_timer.c
 *
 *  Created on: 04.02.2019
 *      Author: nxf29707
 */

#include "stm8s_tim1.h"
#include "stm8s_tim2.h"

void TIM1_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));

  /* set or Reset the CEN Bit */
  if (NewState != DISABLE)
  {
    TIM1->CR1 |= TIM1_CR1_CEN;
  }
  else
  {
    TIM1->CR1 &= (uint8_t)(~TIM1_CR1_CEN);
  }
}


void TIM1_PrescalerConfig(uint16_t Prescaler,
                          TIM1_PSCReloadMode_TypeDef TIM1_PSCReloadMode)
{
  /* Check the parameters */
  assert_param(IS_TIM1_PRESCALER_RELOAD_OK(TIM1_PSCReloadMode));

  /* Set the Prescaler value */
  TIM1->PSCRH = (uint8_t)(Prescaler >> 8);
  TIM1->PSCRL = (uint8_t)(Prescaler);

  /* Set or reset the UG Bit */
  TIM1->EGR = (uint8_t)TIM1_PSCReloadMode;
}


void TIM1_InternalClockConfig(void)
{
  /* Disable slave mode to clock the prescaler directly with the internal clock */
  TIM1->SMCR &= (uint8_t)(~TIM1_SMCR_SMS);
}


void TIM1_TimeBaseInit(uint16_t TIM1_Prescaler,
                       TIM1_CounterMode_TypeDef TIM1_CounterMode,
                       uint16_t TIM1_Period,
                       uint8_t TIM1_RepetitionCounter)
{
  /* Check parameters */
  assert_param(IS_TIM1_COUNTER_MODE_OK(TIM1_CounterMode));

  /* Set the Autoreload value */
  TIM1->ARRH = (uint8_t)(TIM1_Period >> 8);
  TIM1->ARRL = (uint8_t)(TIM1_Period);

  /* Set the Prescaler value */
  TIM1->PSCRH = (uint8_t)(TIM1_Prescaler >> 8);
  TIM1->PSCRL = (uint8_t)(TIM1_Prescaler);

  /* Select the Counter Mode */
  TIM1->CR1 = (uint8_t)((uint8_t)(TIM1->CR1 & (uint8_t)(~(TIM1_CR1_CMS | TIM1_CR1_DIR)))
                        | (uint8_t)(TIM1_CounterMode));

  /* Set the Repetition Counter value */
  TIM1->RCR = TIM1_RepetitionCounter;
}


void TIM1_OC3Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  TIM1_OutputNState_TypeDef TIM1_OutputNState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCNPolarity_TypeDef TIM1_OCNPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState,
                  TIM1_OCNIdleState_TypeDef TIM1_OCNIdleState)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OC_MODE_OK(TIM1_OCMode));
  assert_param(IS_TIM1_OUTPUT_STATE_OK(TIM1_OutputState));
  assert_param(IS_TIM1_OUTPUTN_STATE_OK(TIM1_OutputNState));
  assert_param(IS_TIM1_OC_POLARITY_OK(TIM1_OCPolarity));
  assert_param(IS_TIM1_OCN_POLARITY_OK(TIM1_OCNPolarity));
  assert_param(IS_TIM1_OCIDLE_STATE_OK(TIM1_OCIdleState));
  assert_param(IS_TIM1_OCNIDLE_STATE_OK(TIM1_OCNIdleState));

  /* Disable the Channel 1: Reset the CCE Bit, Set the Output State ,
  the Output N State, the Output Polarity & the Output N Polarity*/
  TIM1->CCER2 &= (uint8_t)(~( TIM1_CCER2_CC3E | TIM1_CCER2_CC3NE |
                             TIM1_CCER2_CC3P | TIM1_CCER2_CC3NP));
  /* Set the Output State & Set the Output N State & Set the Output Polarity &
  Set the Output N Polarity */
  TIM1->CCER2 |= (uint8_t)((uint8_t)((uint8_t)(TIM1_OutputState  & TIM1_CCER2_CC3E   ) |
                                     (uint8_t)(TIM1_OutputNState & TIM1_CCER2_CC3NE  )) |
                           (uint8_t)((uint8_t)(TIM1_OCPolarity   & TIM1_CCER2_CC3P   ) |
                                     (uint8_t)(TIM1_OCNPolarity  & TIM1_CCER2_CC3NP  )));

  /* Reset the Output Compare Bits & Set the Output Compare Mode */
  TIM1->CCMR3 = (uint8_t)((uint8_t)(TIM1->CCMR3 & (uint8_t)(~TIM1_CCMR_OCM)) |
                          (uint8_t)TIM1_OCMode);

  /* Reset the Output Idle state & the Output N Idle state bits */
  TIM1->OISR &= (uint8_t)(~(TIM1_OISR_OIS3 | TIM1_OISR_OIS3N));
  /* Set the Output Idle state & the Output N Idle state configuration */
  TIM1->OISR |= (uint8_t)((uint8_t)(TIM1_OISR_OIS3 & TIM1_OCIdleState) |
                          (uint8_t)(TIM1_OISR_OIS3N & TIM1_OCNIdleState));

  /* Set the Pulse value */
  TIM1->CCR3H = (uint8_t)(TIM1_Pulse >> 8);
  TIM1->CCR3L = (uint8_t)(TIM1_Pulse);
}


void TIM1_OC4Init(TIM1_OCMode_TypeDef TIM1_OCMode,
                  TIM1_OutputState_TypeDef TIM1_OutputState,
                  uint16_t TIM1_Pulse,
                  TIM1_OCPolarity_TypeDef TIM1_OCPolarity,
                  TIM1_OCIdleState_TypeDef TIM1_OCIdleState)
{
  /* Check the parameters */
  assert_param(IS_TIM1_OC_MODE_OK(TIM1_OCMode));
  assert_param(IS_TIM1_OUTPUT_STATE_OK(TIM1_OutputState));
  assert_param(IS_TIM1_OC_POLARITY_OK(TIM1_OCPolarity));
  assert_param(IS_TIM1_OCIDLE_STATE_OK(TIM1_OCIdleState));

  /* Disable the Channel 4: Reset the CCE Bit */
  TIM1->CCER2 &= (uint8_t)(~(TIM1_CCER2_CC4E | TIM1_CCER2_CC4P));
  /* Set the Output State  &  the Output Polarity */
  TIM1->CCER2 |= (uint8_t)((uint8_t)(TIM1_OutputState & TIM1_CCER2_CC4E ) |
                           (uint8_t)(TIM1_OCPolarity  & TIM1_CCER2_CC4P ));

  /* Reset the Output Compare Bit  and Set the Output Compare Mode */
  TIM1->CCMR4 = (uint8_t)((uint8_t)(TIM1->CCMR4 & (uint8_t)(~TIM1_CCMR_OCM)) |
                          TIM1_OCMode);

  /* Set the Output Idle state */
  if (TIM1_OCIdleState != TIM1_OCIDLESTATE_RESET)
  {
    TIM1->OISR |= (uint8_t)(~TIM1_CCER2_CC4P);
  }
  else
  {
    TIM1->OISR &= (uint8_t)(~TIM1_OISR_OIS4);
  }

  /* Set the Pulse value */
  TIM1->CCR4H = (uint8_t)(TIM1_Pulse >> 8);
  TIM1->CCR4L = (uint8_t)(TIM1_Pulse);
}

void TIM1_CtrlPWMOutputs(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));

  /* Set or Reset the MOE Bit */

  if (NewState != DISABLE)
  {
    TIM1->BKR |= TIM1_BKR_MOE;
  }
  else
  {
    TIM1->BKR &= (uint8_t)(~TIM1_BKR_MOE);
  }
}



void TIM1_ITConfig(TIM1_IT_TypeDef  TIM1_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM1_IT_OK(TIM1_IT));
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIM1->IER |= (uint8_t)TIM1_IT;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIM1->IER &= (uint8_t)(~(uint8_t)TIM1_IT);
  }
}


void TIM2_Cmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));

  /* set or Reset the CEN Bit */
  if (NewState != DISABLE)
  {
    TIM2->CR1 |= (uint8_t)TIM2_CR1_CEN;
  }
  else
  {
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_CEN);
  }
}


void TIM2_TimeBaseInit( TIM2_Prescaler_TypeDef TIM2_Prescaler,
                        uint16_t TIM2_Period)
{
  /* Set the Prescaler value */
  TIM2->PSCR = (uint8_t)(TIM2_Prescaler);
  /* Set the Autoreload value */
  TIM2->ARRH = (uint8_t)(TIM2_Period >> 8);
  TIM2->ARRL = (uint8_t)(TIM2_Period);
}


/*void TIM2_TimeBaseInit( TIM2_Prescaler_TypeDef TIM2_Prescaler,
                        uint16_t TIM2_Period)
{
   Set the Prescaler value
  TIM2->PSCR = (uint8_t)(TIM2_Prescaler);
   Set the Autoreload value
  TIM2->ARRH = (uint8_t)(TIM2_Period >> 8);
  TIM2->ARRL = (uint8_t)(TIM2_Period);
}*/


void TIM2_OC1PreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));

  /* Set or Reset the OC1PE Bit */
  if (NewState != DISABLE)
  {
    TIM2->CCMR1 |= (uint8_t)TIM2_CCMR_OCxPE;
  }
  else
  {
    TIM2->CCMR1 &= (uint8_t)(~TIM2_CCMR_OCxPE);
  }
}

void TIM2_SelectOCxM(TIM2_Channel_TypeDef TIM2_Channel, TIM2_OCMode_TypeDef TIM2_OCMode)
{
  /* Check the parameters */
  assert_param(IS_TIM2_CHANNEL_OK(TIM2_Channel));
  assert_param(IS_TIM2_OCM_OK(TIM2_OCMode));

  if (TIM2_Channel == TIM2_CHANNEL_1)
  {
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM2->CCER1 &= (uint8_t)(~TIM2_CCER1_CC1E);

    /* Reset the Output Compare Bits & Set the Output Compare Mode */
    TIM2->CCMR1 = (uint8_t)((uint8_t)(TIM2->CCMR1 & (uint8_t)(~TIM2_CCMR_OCM))
                            | (uint8_t)TIM2_OCMode);
  }
  else if (TIM2_Channel == TIM2_CHANNEL_2)
  {
    /* Disable the Channel 2: Reset the CCE Bit */
    TIM2->CCER1 &= (uint8_t)(~TIM2_CCER1_CC2E);

    /* Reset the Output Compare Bits & Set the Output Compare Mode */
    TIM2->CCMR2 = (uint8_t)((uint8_t)(TIM2->CCMR2 & (uint8_t)(~TIM2_CCMR_OCM))
                            | (uint8_t)TIM2_OCMode);
  }
  else
  {
    /* Disable the Channel 3: Reset the CCE Bit */
    TIM2->CCER2 &= (uint8_t)(~TIM2_CCER2_CC3E);

    /* Reset the Output Compare Bits & Set the Output Compare Mode */
    TIM2->CCMR3 = (uint8_t)((uint8_t)(TIM2->CCMR3 & (uint8_t)(~TIM2_CCMR_OCM))
                            | (uint8_t)TIM2_OCMode);
  }
}


void TIM2_CCxCmd(TIM2_Channel_TypeDef TIM2_Channel, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_TIM2_CHANNEL_OK(TIM2_Channel));
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));

  if (TIM2_Channel == TIM2_CHANNEL_1)
  {
    /* Set or Reset the CC1E Bit */
    if (NewState != DISABLE)
    {
      TIM2->CCER1 |= (uint8_t)TIM2_CCER1_CC1E;
    }
    else
    {
      TIM2->CCER1 &= (uint8_t)(~TIM2_CCER1_CC1E);
    }

  }
  else if (TIM2_Channel == TIM2_CHANNEL_2)
  {
    /* Set or Reset the CC2E Bit */
    if (NewState != DISABLE)
    {
      TIM2->CCER1 |= (uint8_t)TIM2_CCER1_CC2E;
    }
    else
    {
      TIM2->CCER1 &= (uint8_t)(~TIM2_CCER1_CC2E);
    }
  }
  else
  {
    /* Set or Reset the CC3E Bit */
    if (NewState != DISABLE)
    {
      TIM2->CCER2 |= (uint8_t)TIM2_CCER2_CC3E;
    }
    else
    {
      TIM2->CCER2 &= (uint8_t)(~TIM2_CCER2_CC3E);
    }
  }
}

void TIM2_ARRPreloadConfig(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONALSTATE_OK(NewState));

  /* Set or Reset the ARPE Bit */
  if (NewState != DISABLE)
  {
    TIM2->CR1 |= (uint8_t)TIM2_CR1_ARPE;
  }
  else
  {
    TIM2->CR1 &= (uint8_t)(~TIM2_CR1_ARPE);
  }
}


void TIM2_SetAutoreload(uint16_t Autoreload)
{
  /* Set the Autoreload Register value */
  TIM2->ARRH = (uint8_t)(Autoreload >> 8);
  TIM2->ARRL = (uint8_t)(Autoreload);
}


void TIM2_ForcedOC1Config(TIM2_ForcedAction_TypeDef TIM2_ForcedAction)
{
  /* Check the parameters */
  assert_param(IS_TIM2_FORCED_ACTION_OK(TIM2_ForcedAction));

  /* Reset the OCM Bits & Configure the Forced output Mode */
  TIM2->CCMR1  =  (uint8_t)((uint8_t)(TIM2->CCMR1 & (uint8_t)(~TIM2_CCMR_OCM))
                            | (uint8_t)TIM2_ForcedAction);
}



void TIM2_OC1Init(TIM2_OCMode_TypeDef TIM2_OCMode,
                  TIM2_OutputState_TypeDef TIM2_OutputState,
                  uint16_t TIM2_Pulse,
                  TIM2_OCPolarity_TypeDef TIM2_OCPolarity)
{
  /* Check the parameters */
  assert_param(IS_TIM2_OC_MODE_OK(TIM2_OCMode));
  assert_param(IS_TIM2_OUTPUT_STATE_OK(TIM2_OutputState));
  assert_param(IS_TIM2_OC_POLARITY_OK(TIM2_OCPolarity));

  /* Disable the Channel 1: Reset the CCE Bit, Set the Output State , the Output Polarity */
  TIM2->CCER1 &= (uint8_t)(~( TIM2_CCER1_CC1E | TIM2_CCER1_CC1P));
  /* Set the Output State &  Set the Output Polarity  */
  TIM2->CCER1 |= (uint8_t)((uint8_t)(TIM2_OutputState & TIM2_CCER1_CC1E ) |
                           (uint8_t)(TIM2_OCPolarity & TIM2_CCER1_CC1P));

  /* Reset the Output Compare Bits  & Set the Ouput Compare Mode */
  TIM2->CCMR1 = (uint8_t)((uint8_t)(TIM2->CCMR1 & (uint8_t)(~TIM2_CCMR_OCM)) |
                          (uint8_t)TIM2_OCMode);

  /* Set the Pulse value */
  TIM2->CCR1H = (uint8_t)(TIM2_Pulse >> 8);
  TIM2->CCR1L = (uint8_t)(TIM2_Pulse);
}


void TIM2_SetCompare1(uint16_t Compare1)
{
  /* Set the Capture Compare1 Register value */
  TIM2->CCR1H = (uint8_t)(Compare1 >> 8);
  TIM2->CCR1L = (uint8_t)(Compare1);
}
