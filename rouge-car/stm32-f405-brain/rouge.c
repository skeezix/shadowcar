
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>

#include <libopencm3/cm3/cortex.h>

#include "lib_serial.h"
#include "lib_mpu6050_i2c.h"

int main ( void ) {

#if 1 // go for 120MHz, built into libopencm3
  // requires: external 8MHz crystal on pin5/6 with associated caps to ground
  rcc_clock_setup_hse_3v3 ( &hse_8mhz_3v3 [ CLOCK_3V3_120MHZ ] );
#endif

  init_usart1 ( 9600 ); // initialize USART1 @ 9600 baud

  usart_puts ( 1, "Init usart1 complete! Hello World!\r\n" );
  //usart_puts ( 2, "Init usart2 complete! Hello World!\r\n" );

  // MPU setup
  mpu_test();

  //__enable_irq();
  //cm_enable_interrupts();

  while ( 1 ) {
    __asm__("nop");
  } // while forever

  return 0;
}
