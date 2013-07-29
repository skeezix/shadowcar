#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include <avr/io.h>
#include <avr/interrupt.h>   // include interrupt support
#include "lcd_drv.h"

// adafruit lcd wiring tutorial: http://learn.adafruit.com/character-lcds/wiring-a-character-lcd
// install path: /usr/lib/avr/include/avr

// globals: timer
volatile unsigned int g_timer_us = 0; // will overflow and cycle after 6 seconds
volatile unsigned int g_time_s = 0;   // will overflow after 16 hours
volatile unsigned int _g_time_us_tick = 0; // internal ticker to count seconds passage; ms accrue since last sec bump

// globals: receiver
volatile unsigned char g_ch1_duration = 0; // us that last pwm was at (11-19/20, 15 is at rest) 11left<->right
volatile unsigned char g_ch2_duration = 0; // us that last pwm was at (11-19/20, 15 is at rest) up<->down

int main( void )
{
  uint32_t val = 0;
  char textbuf [ (2*16) + 1 ];

  /* setup
   */

  // LCD
  lcd_init();

  // enable a timer so we can measure passage of time
  // Given: 20MHz clock
  // --> if we want resolution of ms (1/1000th second) .. actually, we want us (1/10th of a ms) so we can measure partial ms
  // --> and we have 1/20000000 clock resolution
  // -----> 2000 ticks will get us there (20,000 will get us ms)
  //
  // Goal: Use CTC interupt mode (CTC -> Clear on Timer Compare)
  // So a compare matches, it clears to zero and triggers interupt
  TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode 
  OCR1A = 2000; // number to compare against
  TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt 
  TCCR1B |= (1 << CS10); // Set up timer , with no prescaler (works at full MHz of clock)

#if 1 // set up pin change interupt
  EICRA &= ~ ( (1 << ISC01) | (1 << ISC01) ); // clear ISC01+ISC00
  EICRA |= ( (1 << ISC00) ); // 00 set and 01 unset means any edge will make event
  PCMSK0 |= ( (1 << PCINT0) | (1 << PCINT1) ); // Pins to monitor: PA0 and PA1
  PCICR |= (1 << PCIE0); // PA is monitored
#endif

  // setup done - kick up interupts
  sei();

#if 1 // timer test .. show per-second counter update on lcd
  if ( 1 ) {
    unsigned int last_sec = g_time_s;

    while(1) {

      if ( g_time_s != last_sec ) {
        sprintf ( textbuf, "%2d %2d     ", g_ch1_duration, g_ch2_duration );
        lcd_xy ( 0, 0 );
        lcd_puts( textbuf ); // display number right adjusted

        sprintf ( textbuf, "timer %2d #", g_time_s );
        lcd_xy ( 0, 1 );
        lcd_puts( textbuf ); // display number right adjusted

        last_sec = g_time_s;
      }

    } // while forever

  } // if 1
#endif

#if 1 // RC receiver -> PWM -> pulse edge detection via interupt .. approach
#endif

#if 0 // ADC approach for RC receiver
  // ADC
  // -> ADC prescaler to 128 (20MHz div by 128 is 156KHz which is within the 50-200 required)
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  // -> set reference voltage to VCC
  ADMUX = 0;
  ADMUX |= (1 << REFS0);
  //ADMUX &= ~(1<<REFS1);
  // -> set ADC0 as active one; this is default
  //ADMUX &= ~(0x0F); // clear or ADC0
  //ADMUX |= 1; // ADC1
  //ADMUX
  // -> enable free-running mode (run continuously, not single sample)
  //ADCSRA |= (1 << ADFR);
  ADCSRA |= (1<<ADATE); // rising edge is trigger; must be set for ADTSx to be used
  ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0)); // clear these 3 == free running
  // -> default is 10bit value split across L (low) and H (high 2bits); shift it all into H so
  //    we get easier to use 8bit resolution instead of 10
  //ADMUX |= (1 << ADLAR); 
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

  unsigned char counter = 0;
  unsigned char low, hi, low1;
  char dir [ 5 ] = { '\0', '\0', '\0', '\0', '\0' };
  elevon_e axis = el_nil;

  if ( 1 ) {

    while ( 1 ) {

      // ADC0 - left/right axis
      ADMUX &= ~(0x0F);

      // delay
      _delay_ms ( 200 );

      low = ADCL;
      hi = ADCH;

      low1 = low;

      if ( low < 26 ) {
        dir [ 0 ] = '<'; dir [ 1 ] = '<';
      } else if ( low < 31 ) {
        dir [ 0 ] = '<'; dir [ 1 ] = '_';
      } else if ( low > 44 ) {
        dir [ 0 ] = '>'; dir [ 1 ] = '>';
      } else if ( low > 39 ) {
        dir [ 0 ] = '>'; dir [ 1 ] = '_';
      } else {
        dir [ 0 ] = '_'; dir [ 1 ] = '_';
      }

      // ADC1 - up/down axis
      ADMUX &= ~(0x0F);
      ADMUX |= 1;

      // delay
      _delay_ms ( 200 );

      low = ADCL;
      hi = ADCH;

      if ( low < 29 ) {
        dir [ 2 ] = 'v'; dir [ 3 ] = 'v';
      } else if ( low < 33 ) {
        dir [ 2 ] = 'v'; dir [ 3 ] = '_';
      } else if ( low > 45 ) {
        dir [ 2 ] = '^'; dir [ 3 ] = '^';
      } else if ( low > 41 ) {
        dir [ 2 ] = '^'; dir [ 3 ] = '_';
      } else {
        dir [ 2 ] = '_'; dir [ 3 ] = '_';
      }

      sprintf ( textbuf, "%3d| %2d %2d %s#", counter, low1, low, dir );

      lcd_xy( 0, 0 );
      lcd_puts( textbuf ); // display number right adjusted

      _delay_ms ( 200 );
      counter ++;

    } // while

  } // if1
#endif

  /* churn forever
   */
  while(1);

  return ( 0 );
}

// ISR for timer1 ticking away
ISR(TIMER1_COMPA_vect)
{
  // bump ms timer counter
  g_timer_us++;

  // bump 'seconds' counter as well
  _g_time_us_tick++;
  if ( _g_time_us_tick > 10000 ) {
    _g_time_us_tick = 0;
    g_time_s ++;
  }

} // ISR for timer1

// ISR for pin change events
volatile unsigned int g_ch1_rose = 0; // time that channel1 rose
volatile unsigned int g_ch2_rose = 0; // time that channel1 rose
volatile unsigned char _g_pin_state = 0; // so we can know which pins changed since last interupt
ISR(PCINT0_vect)
{
  unsigned int timer = g_timer_us;
  unsigned char delta = _g_pin_state ^ PINA;

  if ( delta & ( 1 << PA0 ) ) {

    // CH1: edge has gone up, or down?
    if ( PINA & ( 1 << PA0 ) ) {
      // edge has gone high
      g_ch1_rose = timer;
    } else {
      // handle overflow too..
      if ( g_ch1_rose < timer ) {
        g_ch1_duration = timer - g_ch1_rose;
      } else {
        // overflow.. ignore result, who cares if we miss one once in awhile
      }
    }

  } // PA0 changed?

  // COPY PASTE from above, changing channel
  if ( delta & ( 1 << PA1 ) ) {

    // CH2: edge has gone up, or down?
    if ( PINA & ( 1 << PA1 ) ) {
      // edge has gone high
      g_ch2_rose = timer;
    } else {
      // handle overflow too..
      if ( g_ch2_rose < timer ) {
        g_ch2_duration = timer - g_ch2_rose;
      } else {
        // overflow.. ignore result, who cares if we miss one once in awhile
      }
    }

  } // PA1 changed?

  // update last state
  _g_pin_state = PINA;
}
