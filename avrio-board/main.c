#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "lcd_drv.h"

// adafruit lcd wiring tutorial: http://learn.adafruit.com/character-lcds/wiring-a-character-lcd
// install path: /usr/lib/avr/include/avr

int main( void )
{
  uint32_t val = 0;
  char s [ sizeof("4294967295") ]; // ASCII space for 32 bit number
  char textbuf [ 16 + 1 ];

  /* setup
   */

  // LCD
  lcd_init();

  // ADC
  // -> ADC prescaler to 128 (20MHz div by 128 is 156KHz which is within the 50-200 required)
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  // -> set reference voltage to VCC
  ADMUX = 0;
  ADMUX |= (1 << REFS0);
  //ADMUX &= ~(1<<REFS1);
  // -> set ADC0 as active one; this is default
  //ADMUX
  // -> enable free-running mode (run continuously, not single sample)
  //ADCSRA |= (1 << ADFR);
  ADCSRA |= (1<<ADATE); // rising edge is trigger; must be set for ADTSx to be used
  ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0)); // clear these 3 == free running
  // -> default is 10bit value split across L (low) and H (high 2bits); shift it all into H so
  //    we get easier to use 8bit resolution instead of 10
  ADMUX |= (1 << ADLAR); 
  // -> digital input pin disable . save on power use
  DIDR0 |= (1<<ADC0D);

  /* startup
   */

  // ADC
  // -> enable ADC
  ADCSRA |= (1 << ADEN);
  // -> start measuring
  ADCSRA |= (1 << ADSC);
  // -> read value
  //    should start showing up in ADCH

  /* do something useful..
   */

#if 1
  unsigned char counter = 0;
  unsigned char low, hi;

  if ( 1 ) {

    while ( 1 ) {
      low = ADCL;
      hi = ADCH;
      //word = ADCW;

      sprintf ( textbuf, "%2d -> %2d %2d #", counter, hi, low );

      lcd_xy( 0, 0 );
      lcd_puts( textbuf ); // display number right adjusted

      _delay_ms ( 500 );
      counter ++;

    } // while

  } // if1
#endif

  /* churn forever
   */
  while(1);

  return ( 0 );
}
