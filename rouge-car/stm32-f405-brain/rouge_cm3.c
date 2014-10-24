
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>

#include <libopencm3/cm3/cortex.h>

#include "lib_serial.h"
#include "lib_mpu6050_i2c.h"
#include "rouge_cm3.h"
#include "lib_pwm_motor.h"
#include "skeelib.h"
#include "balancer.h"

int main_cm3 ( void ) {

#if 1 // go for 120MHz, built into libopencm3
  // requires: external 8MHz crystal on pin5/6 with associated caps to ground
  rcc_clock_setup_hse_3v3 ( &hse_8mhz_3v3 [ CLOCK_3V3_120MHZ ] );
#endif

  // wait a sec, so we have a reflash opportunity before things get funky
  //
  lame_delay_ms ( 2000 );

  // initialize USART1 @ 9600 baud
  //
  init_usart1 ( 9600 );

  usart_puts ( 1, "Init usart1 complete! Hello World!\r\n" );
  //usart_puts ( 2, "Init usart2 complete! Hello World!\r\n" );

  // PWM setup
#if 0
  usart_puts ( 1, "usart1 pwm test start!\r\n" );
  pwm_test();
  usart_puts ( 1, "usart1 pwm test complete!\r\n" );
#endif

  // MPU setup
#if 0
  mpu_test();
  usart_puts ( 1, "usart1 mpu test complete!\r\n" );
#endif

  // balance!
#if 1
  usart_puts ( 1, "usart1 - enter balance mode!\r\n" );
  balance_or_die();
#endif

  //__enable_irq();
  //cm_enable_interrupts();

  while ( 1 ) {
    __asm__("nop");
  } // while forever

  return 0;
}
