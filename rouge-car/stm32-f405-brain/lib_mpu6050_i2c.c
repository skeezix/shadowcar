
#include "MPU6050.h"
#include "lib_mpu6050_i2c.h"
#include "lib_serial.h"
#include "skeelib.h"

unsigned char mpu_test ( void ) {
  usart_puts ( 1, "lib MPU6050 - test - enter!\r\n" );

  usart_puts ( 1, "lib MPU6050 - test - i2c init!\r\n" );
  MPU6050_I2C_Init();

  usart_puts ( 1, "lib MPU6050 - test - mpu init!\r\n" );
  MPU6050_Initialize();

  usart_puts ( 1, "lib MPU6050 - test - test connection!\r\n" );

  if ( MPU6050_TestConnection() == FALSE ) {
    usart_puts ( 1, "Init MPU6050 - FAIL!\r\n" );
    return ( 0 );
  }

  // connection success
  usart_puts ( 1, "Init MPU6050 - success\r\n" );

  // run a looping test
  //
  int16_t AccelGyro[6]={0};
  unsigned char i;
  char b [ 20 ];

  while ( 1 ) {

    MPU6050_GetRawAccelGyro ( AccelGyro );

    usart_puts ( 1, "GyroAccel: " );
    for ( i = 0; i <= 5; i++ ) {
      lame_itoa ( AccelGyro [ i ], b );
      usart_puts ( 1, b );
      usart_puts ( 1, " , " );
    }
    usart_puts ( 1, "\r\n" );

    lame_delay_ms ( 1000 );
  } // while

  return ( 1 );
}
