
#include <stm32f4xx.h>
#include <stm32f4xx_tim.h>

#include "lib_pwm_motor.h"
#include "MPU6050.h"
#include "lib_mpu6050_i2c.h"
#include "lib_serial.h"
#include "balancer.h"
#include "skeelib.h"

void balance_or_die ( void ) {

  // set up MPU6050
  MPU6050_I2C_Init();
  MPU6050_Initialize();

  if ( MPU6050_TestConnection() == FALSE ) {
    usart_puts ( 1, "Init MPU6050 - FAIL!\r\n" );
    while ( 1 ); // spin forever
  }

  // set up motors
  pwm_init_steerage();
  pwm_init_tim8 ( 500 );

  // balance forever..
  int16_t AccelGyro[6]={0};
  int16_t tip;

  while ( 1 ) {

    MPU6050_GetRawAccelGyro ( AccelGyro );
    tip = AccelGyro [ 1 ];

    // AccelGyro [ 1 ]
    // -16000 - flat down normal
    //   -12000 - about 45 up
    //   -9000 - about right center of gravity ..
    // -2500 - about vertical
    // +ve - so far up its falling on its back

    if ( tip < -12000 ) {
      // nothing .. we're flat

      pwm_set_left_direction ( 1 );
      pwm_set_right_direction ( 1 );
      pwm_set_left_speed ( 254 );
      pwm_set_right_speed ( 254 );

    } else if ( tip < -8700 ) {
      // trying to flatten down, run backwards to counter

      pwm_set_left_direction ( -1 );
      pwm_set_right_direction ( -1 );
      pwm_set_left_speed ( 254 );
      pwm_set_right_speed ( 254 );

    } else if ( tip > -8400 ) {
      // trying to fall back, run forewards to counter

      pwm_set_left_direction ( 1 );
      pwm_set_right_direction ( 1 );
      pwm_set_left_speed ( 254 );
      pwm_set_right_speed ( 254 );

    } else {
      // nothing

      pwm_set_left_direction ( 0 );
      pwm_set_right_direction ( 0 );
      pwm_set_left_speed ( 0 );
      pwm_set_right_speed ( 0 );

    }

    lame_delay_ms ( 1 );
  } // while balancing

}
