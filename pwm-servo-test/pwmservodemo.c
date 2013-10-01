
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

// install path: /usr/lib/avr/include/avr

#define F_CPU 1000000UL  /* 1 MHz CPU clock */
//#define F_CPU 8000000UL  /* 8 MHz CPU clock */
//#define F_CPU 20000000UL  /* 20 MHz CPU clock */

#include <util/delay.h>
#include <avr/io.h>

// direction
//   ADR (analog direction register) and DDR (digital direction register)
//   DDRx -> set value to 1, sets dir to write; low is read
//   PORTxn -> set high; if write, sets to high; if read, sets pull up resistor
// interupts
//   GIMSK = _BV (INT0); // int - Enable external interrupts int0
//   MCUCR = _BV (ISC01); // int - INT0 is falling edge
//   sei(); // int - Global enable interrupts

#define SERVOTIMER 1
#define SERVO1 1
#define SERVO2 1

int main ( void ) {

  // Given:
  // - Timer1 is used in avrio-board already for keeping track of time (in turn which
  //   is used for counting up/down duration on pwm from receiver.)
  //

  // PWM setup steps:
  // http://aquaticus.info/pwm-frequency
  // 1. Setup OCx pin as output
  // 2. Select PWM mode of timer
  // 3. Set appropriate prescaler divider
  // 4. Set Compare Output Mode to Clear or Set on compare match
  // 5. Write duty cycle value to OCRx

  // atmega644 OC (on compare) pins
  // OC0A and OC0B -> pin 4 PB3 and 5 PB4
  // OC1A and OC1B -> pin 19 PD5 and 18 PD4
  // OC2A and OC2B -> pin 21 PD7 and pin 20 PD6

  // blinking LED
  DDRB |= (1<<PB0); // Poor choice, this is one of our timer pins :P But it is near the end, which is handy :P



#ifdef SERVOTIMER
  // PWM mode (normal, fast pwm, phase corrected, etc)
  // just 0 -> PWM Phase Corrected, top 0xFF
  // 2 and 0 -> PWM Phase Corrected, top OCRA
  // just 1 -> CTC
  TCCR2A &= ~( (1<<WGM20) | (1<<WGM21) );
  TCCR2B &= ~( (1<<WGM22) );
  TCCR2A |= (1<<WGM20); // PWM Phase Correct
  //TCCR2A |= (1<<WGM21); // PWM Phase Correct
  //TCCR2A |= (1<<WGM22); // PWM Phase Correct
  // clock source
  TCCR2B &= ~( (1<<CS22) | (1<<CS21) | (1<<CS20) ); // clear all (no clock)
  TCCR2B |= (1<<CS20); // turn on clock
  TCCR2B |= (1<<CS21); // turn on clock .. 22+21 == 256 prescaler
  //TCCR2B |= (1<<CS22); // turn on clock
#endif





#ifdef SERVO1
  // PWM 1 - OC2A PD7 - Timer 2A
  //
  // direction -> output
  DDRD |= (1<<PD7);
  // timer setup
  //   0 1 Toggle OC2A on Compare Match
  TCCR2A &= ~(1<<COM2A1);
  TCCR2A &= ~(1<<COM2A0);
  //TCCR2A |= (1<<COM2A0);
  TCCR2A |= (1<<COM2A1);
  //   1 0 Clear OC2A on Compare Match
  //TCCR2A &= ~(1<<COM2A1);
  //TCCR2A |= (1<<COM2A0);

  // duty cycle
#if 0
  // 0 -> off
  // 1 -> minimum, about 50uS
  // 15 -> 1mS
  // 22 -> 1.5mS
  // 30 -> 2mS

  // 180deg servo
  // 9 -> far right in current demo -> just over half mS (?!)
  // 39 or 40 -> far left -> 2.5mS (?!)
  // 24 is middle (24 +- 15)

  OCR2A = 24;
#endif

#endif // SERVO1






#ifdef SERVO2
  // PWM 2 - OC2B PD6 - Timer 2B
  // direction -> output
  DDRD |= (1<<PD6);
  // timer setup
  //   0 1 Toggle OC2A on Compare Match
  TCCR2A &= ~(1<<COM2B1);
  TCCR2A &= ~(1<<COM2B0);
  //TCCR2A |= (1<<COM2A0);
  TCCR2A |= (1<<COM2B1);
  //   1 0 Clear OC2A on Compare Match
  //TCCR2A &= ~(1<<COM2A1);
  //TCCR2A |= (1<<COM2A0);
  // .. no WGM or clock source control here; there is only 1 timer, so we can't set it differently to above
#if 1
  OCR2B = 36; // rest
  //OCR2B = 28; // about max looking up that we want to do (verified ok)
  //OCR2B = 40; // about max looking forward/down that we want to do (verified ok)
#endif

#endif // SERVO2

#ifdef SERVO1 // servo 1 (bottom)
  signed char dir = 1;
  signed char pwm = 24;
#endif

#ifdef SERVO2 // servo 2 (top)
  unsigned char divider = 0;
  signed char dir2 = 1;
  signed char pwm2 = 36;
#endif

  while ( 1 ) {

    PORTB &= ~ (1<<PB0);
    _delay_ms ( 45 );

    PORTB |= (1<<PB0);
    _delay_ms ( 45 );

#ifdef SERVO1 // servo 1
    if ( pwm == 9 ) { // 9
      dir = 1;
    } else if ( pwm == 39 ) { // 39
      dir = -1;
    }

    if ( dir > 0 ) {
      pwm++;
    } else {
      pwm--;
    }

    OCR2A = pwm;
#endif

#ifdef SERVO2 // servo 2
    if ( divider < 2 ) {
      divider++;
      continue;
    } else {
      divider = 0;
    }

    if ( pwm2 == 28 ) { // 28
      dir2 = 1;
    } else if ( pwm2 == 40 ) { // 40
      dir2 = -1;
    }

    if ( dir2 > 0 ) {
      pwm2++;
    } else {
      pwm2--;
    }

    OCR2B = pwm2;
#endif

  } // while forever

  return (0);
}
