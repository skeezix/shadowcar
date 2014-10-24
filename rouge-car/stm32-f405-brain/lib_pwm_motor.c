
#include <stm32f4xx.h>
#include <stm32f4xx_tim.h>

#include "lib_pwm_motor.h"
#include "lib_serial.h"
#include "skeelib.h"

#if 0 // TEST: set PC10 gpio to output, toggle it
  GPIO_InitTypeDef gpioStructureTest;
  gpioStructureTest.GPIO_Pin = GPIO_Pin_10;
  gpioStructureTest.GPIO_Mode = GPIO_Mode_OUT;
  gpioStructureTest.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &gpioStructureTest);

  GPIO_WriteBit ( GPIOC, GPIO_Pin_10, Bit_SET );

  while(1) {
    usart_puts ( 1, "usart1 pwm test doit!\r\n" );
    GPIO_ToggleBits ( GPIOC, GPIO_Pin_10 );
    lame_delay_ms ( 10 );
  }
#endif

void pwm_init_steerage ( void ) {

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; // 14
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init ( GPIOA, &GPIO_InitStructure );
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; // 15
  GPIO_Init ( GPIOA, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // 10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init ( GPIOC, &GPIO_InitStructure );
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // 11
  GPIO_Init ( GPIOC, &GPIO_InitStructure );

  GPIO_ResetBits ( GPIOA, GPIO_Pin_14 | GPIO_Pin_15 );
  GPIO_ResetBits ( GPIOC, GPIO_Pin_10 | GPIO_Pin_11 );

}

void pwm_set_left_direction ( signed char dir ) {

  if ( dir < 0 ) {
    GPIO_ResetBits ( GPIOA, GPIO_Pin_14 );
    GPIO_SetBits ( GPIOA, GPIO_Pin_15 );
  } else if ( dir > 0 ) {
    GPIO_ResetBits ( GPIOA, GPIO_Pin_15 );
    GPIO_SetBits ( GPIOA, GPIO_Pin_14 );
  } else {
    GPIO_ResetBits ( GPIOA, GPIO_Pin_14 | GPIO_Pin_15 );
  }

}

void pwm_set_left_speed ( unsigned char speed ) {
  unsigned int pulse = ((unsigned int)speed) * 500 / 255;

#if 0
  char b [ 20 ];
  usart_puts ( 1, "left speed " );
  lame_itoa ( pulse, b );
  usart_puts ( 1, b );
  usart_puts ( 1, "\r\n" );
#endif

  pwm_init_pc8_tim8_ch3 ( pulse );
}

void pwm_set_right_direction ( signed char dir ) {

  if ( dir < 0 ) {
    GPIO_ResetBits ( GPIOC, GPIO_Pin_10 );
    GPIO_SetBits ( GPIOC, GPIO_Pin_11 );
  } else if ( dir > 0 ) {
    GPIO_ResetBits ( GPIOC, GPIO_Pin_11 );
    GPIO_SetBits ( GPIOC, GPIO_Pin_10 );
  } else {
    GPIO_ResetBits ( GPIOC, GPIO_Pin_10 | GPIO_Pin_11 );
  }

}

void pwm_set_right_speed ( unsigned char speed ) {
  unsigned int pulse = ((unsigned int)speed) * 500 / 255;
  pwm_init_pc9_tim8_ch4 ( pulse );
}

void pwm_init_tim8 ( unsigned int period ) {

  // TIM8 CH4 PC9
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
 
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = (168000000 / 1000000) - 1; // Get clock to 1 MHz on STM32F2/F4
  TIM_TimeBaseStructure.TIM_Period = period;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit ( TIM8, &TIM_TimeBaseStructure );
 
  /* Enable TIM1 Preload register on ARR */
  TIM_ARRPreloadConfig ( TIM8, ENABLE );
 
   /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM8, ENABLE);
 
  /* TIM1 enable counter */
  TIM_Cmd(TIM8, ENABLE);

}

void pwm_init_pc8_tim8_ch3 ( unsigned int pulse ) {

  // TIM8 CH4 PC9
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init ( GPIOC, &GPIO_InitStructure );
 
  /* Connect TIM1 pins to AF */
  GPIO_PinAFConfig ( GPIOC, GPIO_PinSource8, GPIO_AF_TIM8 );

  TIM_OCInitTypeDef  TIM_OCInitStructure;
 
  /* TIM PWM1 Mode configuration */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = pulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
 
  /* Output Compare PWM1 Mode configuration: Channel1 PA.08 */
  TIM_OC3Init ( TIM8, &TIM_OCInitStructure );
  TIM_OC3PreloadConfig ( TIM8, TIM_OCPreload_Enable );
 
}

void pwm_init_pc9_tim8_ch4 ( unsigned int pulse ) {

  // TIM8 CH4 PC9
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init ( GPIOC, &GPIO_InitStructure );
 
  /* Connect TIM1 pins to AF */
  GPIO_PinAFConfig ( GPIOC, GPIO_PinSource9, GPIO_AF_TIM8 );

  TIM_OCInitTypeDef  TIM_OCInitStructure;
 
  /* TIM PWM1 Mode configuration */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = pulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
 
  /* Output Compare PWM1 Mode configuration: Channel1 PA.08 */
  TIM_OC4Init ( TIM8, &TIM_OCInitStructure );
  TIM_OC4PreloadConfig ( TIM8, TIM_OCPreload_Enable );
 
}


int pwm_test ( void ) {


  // cm3 toggle test
  /*
  rcc_periph_clock_enable(RCC_GPIOC);

  gpio_mode_setup ( GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10 );

  while (1) {
    gpio_toggle(GPIOC, GPIO10);
    lame_delay_ms ( 20 );
  }
  */

  pwm_init_steerage();

  unsigned int period;
  unsigned char speed;
  signed char dir = 1;
  while ( 1 ) {
    period = 500;
    speed = 100;

    pwm_init_tim8 ( period );
    pwm_set_left_direction ( dir );
    pwm_set_right_direction ( dir );

    if ( dir > 0 ) {
      dir = -1;
    } else {
      dir = 1;
    }

    while ( speed < 255 ) {
      speed += 1;

      pwm_set_left_speed ( speed );
      pwm_set_right_speed ( speed );

      if ( speed == 253 ) {
        lame_delay_ms ( 2000 );
      }

      lame_delay_ms ( 5 );
    }

  }

  return ( 0 );
}
