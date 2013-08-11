#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef LINUX
#include "main.h"
#include <avr/io.h>
#include <avr/interrupt.h>   // include interrupt support
#include <util/setbaud.h>
#include "lcd_drv.h"
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include "mc_sabertooth.h"

unsigned char mc_setup ( void ) {
#ifndef LINUX

  // set up the UART to the right baud rate (as defined in BAUD header)
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
  UCSR0A &= ~(1 << U2X0);
#endif

  // date size
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */ 

  // supported functions
  //UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
  UCSR0B |= ( 1 << TXEN0 );   /* Enable TX */

#endif // Linux
  return;
}

void mc_speed ( mc_motor_select_e select, unsigned char strength ) {

#ifdef LINUX
  char motor [ 32 ];

  switch ( select ) {
  case mcm_left:
    sprintf ( motor, "Left" );
    break;
  case mcm_right:
    sprintf ( motor, "Right" );
    break;
  case mcm_both:
    sprintf ( motor, "Both" );
    break;
  case mcm_allstop:
    sprintf ( motor, "AllStop" );
    break;
  default:
    sprintf ( motor, "ERROR" );
  }

  printf ( "\t->\tmotor %8s\tstrength %d\n", motor, strength );
#else

  switch ( select ) {

  case mcm_nil:
    break;

  case mcm_left:
    uart_putchar_prewait ( strength ); // motor 1
    break;

  case mcm_right:
    uart_putchar_prewait ( strength + 127 ); // motor 2
    break;

  case mcm_both:
    uart_putchar_prewait ( strength );       // motor 1
    uart_putchar_prewait ( strength + 127 ); // motor 2
    break;

  case mcm_allstop:
    uart_putchar_prewait ( 0 );

  } // switch

#endif // Linux
  return;
}

static
void mc_set_by_receiver_inc ( unsigned char x, unsigned char y,
                              unsigned char *r_sent_l, unsigned char *r_sent_r,
                              char *r_message )
{
  // incrementing mode..
  //
  // - degree of tilt == delta-amount
  //   - no tilt == no adjustment (stay current orders)
  // - apply delta amount, until hitting cap or floor
  // - left/right applies negative to that side (towards floor)
  // - when landing at stop, require a release and then repull to pass it?
  // - Special Case: if request is turn left/right, and current forward speed is 0 and no Y throttle, then turn on spot
  static unsigned char strength = 0; // 0 min -> 62 max
  static unsigned char forward = 1;  // >0 -> forward, otherwise reverse

  // allow x/y values 10..20 (expecting 11..19, but 10/20 do occasionally happen; treat same.)
  x = MIN(19,x); // if 20, goes to 19
  x = MAX(11,x); // if 10, goes to 11
  y = MIN(19,y); // if 20, goes to 19
  y = MAX(11,y); // if 10, goes to 11
  // normalized to --> 11..19 with 14/15/16 as dead

  signed char delta_throttle;

  // handle Y tilt
  //

  // map tilt to delta
  delta_throttle = 0;

  if ( y > DEADCEIL ) {
    delta_throttle = 1;
  } else if ( y < DEADFLOOR ) {
    delta_throttle = -1;
  }

#if 0
  if ( delta_throttle && ! forward ) {
    delta_throttle *= -1; // if going reverse, invert the direction
  }
#endif

  // short circuit special case
  if ( ( delta_throttle == 0 ) && // Y is not tilted
       ( ( x < DEADFLOOR ) || ( x > DEADCEIL ) )  // X _is_ tiled
     )
  {
    // turn on the spot

    unsigned char right;
    unsigned char spin;

    // determine initial strength based on throttle fwd/rev
    if ( x > DEADCEIL ) {
      right = 1;
      spin = x - DEADCEIL; // 1..3+ .. 1 being least
    } else {
      right = 0;
      spin = DEADFLOOR - x; // 1..3+ .. 1 being least
    }

    // convert strength 1..3+ to relative strength value
    // 15 is not enough to turn..
    switch ( spin ) {
    case 1:   spin = 25; break;
    case 2:   spin = 35; break;
    default:  spin = 45;
    }

    // set speeds
    if ( right ) {
      mc_speed ( mcm_right, MOTOROFF + spin );
      mc_speed ( mcm_left, MOTOROFF - spin );
    } else {
      mc_speed ( mcm_left, MOTOROFF + spin );
      mc_speed ( mcm_right, MOTOROFF - spin );
    }

    return;
  } // short circuit to turn on spot?

  // apply delta .. stopping at 0
  if ( delta_throttle != 0 ) {

    if ( strength == 0 ) {

      // determine new direction..
      if ( delta_throttle > 0 ) {
        forward = 1;
      } else if ( delta_throttle < 0 ) {
        forward = 0;
      }

    } // str==0?

    if ( delta_throttle < 0 ) {

      figure_this_out;
      if ( forward ) {
      } else {
      }

      if ( abs ( delta_throttle ) > strength ) {
        strength = 0;
      } else {
        strength -= abs ( delta_throttle );
      }

    } else if ( delta_throttle > 0 ) {

      figure_this_out;
      if ( forward ) {
      } else {
      }

      if ( strength + delta_throttle < 62 ) {
        strength += delta_throttle;
      } else {
        strength = 62;
      }

    } // delta direction?

  } // throttling?

  // break into components
  unsigned char str_l = strength, str_r = strength;
  unsigned char turn = 0;

  // apply left/right bias
  //

  // determine initial strength based on throttle fwd/rev
  if ( x > DEADCEIL ) {
    turn = x - DEADCEIL; // 1..3 .. 1 being least
  } else if ( x < DEADFLOOR ) {
    turn = DEADFLOOR - x; // 1..3 .. 1 being least
  }

  // convert turn strength 1..3 to relative turn strength value
  // 15 is not enough to turn..
#if 0
  switch ( turn ) {
  case 0:   turn = 0; break;
  case 1:   turn = 25; break;
  case 2:   turn = 35; break;
  default:  turn = 45;
  }
#endif

  // apply bias
  if ( turn ) {

    if ( x > DEADCEIL ) {
      // turn right; reduce right motor
      str_r /= ( 1 + turn );
    } else {
      // turn left; reduce left motor
      str_l /= ( 1 + turn );
    }

  }

  // apply motor settings
  //

  // apply motor change
  // handle direction
  if ( forward ) { 
    mc_speed ( mcm_left, MOTOROFF + str_l );
    mc_speed ( mcm_right, MOTOROFF + str_r );
  } else {
    mc_speed ( mcm_left, MOTOROFF - str_l );
    mc_speed ( mcm_right, MOTOROFF - str_r );
  }

  return;
}

static
void mc_set_by_receiver_abs ( unsigned char x, unsigned char y,
                              unsigned char *r_sent_l, unsigned char *r_sent_r,
                              char *r_message )
{

  // lame, but for now..
  // Y -> if pure Y (X is dead zone), then forward or back various speed; ie: left == right
  // X (if X not in dead zone) .. rotate on spot various speed; ie: left = -right.

  // check dead space; which are we doing.. turning, or moving?
  if ( mc_is_deadspace ( x ) ) {
    // possible fwd/back
    unsigned char forward;
    unsigned char strength;

    // is Y deadspace? if so...
    if ( mc_is_deadspace ( y ) ) {
      mc_speed ( mcm_allstop, 0 );
      return;
    }

    // we must be intending forward or back!
    //

    // determine initial strength based on throttle fwd/rev
    if ( y > DEADCEIL ) {
      forward = 1;
      strength = y - DEADCEIL; // 1..3+ .. 1 being least
    } else {
      forward = 0;
      strength = DEADFLOOR - y; // 1..3+ .. 1 being least
    }

    // convert strength 1..3+ to relative strength value
    switch ( strength ) {
    case 1:   strength = 5; break;
    case 2:   strength = 20; break;
    default:  strength = 40;
    }

    // handle direction
    if ( forward ) { 
      strength = MOTOROFF + strength;
    } else {
      strength = MOTOROFF - strength;
    }

    // set speeds
    mc_speed ( mcm_both, strength );

    if ( r_sent_l ) {
      *r_sent_l = strength;
    }
    if ( r_sent_r ) {
      *r_sent_r = strength;
    }
    if ( r_message ) {
      strcpy ( r_message, "Y" );
    }

  } else {
    // intention to turn on spot
    unsigned char right;
    unsigned char strength;

    // determine initial strength based on throttle fwd/rev
    if ( x > DEADCEIL ) {
      right = 1;
      strength = x - DEADCEIL; // 1..3+ .. 1 being least
    } else {
      right = 0;
      strength = DEADFLOOR - x; // 1..3+ .. 1 being least
    }

    // convert strength 1..3+ to relative strength value
    // 15 is not enough to turn..
    switch ( strength ) {
    case 1:   strength = 25; break;
    case 2:   strength = 35; break;
    default:  strength = 45;
    }

#if 0
    // handle direction
    if ( right ) { 
      strength = MOTOROFF + strength;
    } else {
      strength = MOTOROFF - strength;
    }
#endif

    // set speeds
    if ( right ) {
      mc_speed ( mcm_right, MOTOROFF + strength );
      mc_speed ( mcm_left, MOTOROFF - strength );
    } else {
      mc_speed ( mcm_left, MOTOROFF + strength );
      mc_speed ( mcm_right, MOTOROFF - strength );
    }

#if 0
    if ( r_sent_l ) {
      *r_sent_l = strength;
    }
    if ( r_sent_r ) {
      *r_sent_r = strength;
    }
    if ( r_message ) {
      strcpy ( r_message, "X" );
    }
#endif

  } // Y deadspace -> mode

  return;
}

void mc_set_by_receiver ( unsigned char x, unsigned char y,
                          unsigned char *r_sent_l, unsigned char *r_sent_r,
                          char *r_message )
{
#ifdef LINUX
  printf ( "receiver %d %d\n", x, y );
#endif

  //mc_set_by_receiver_abs ( x, y, r_sent_l, r_sent_r, r_message );
  mc_set_by_receiver_inc ( x, y, r_sent_l, r_sent_r, r_message );

  return;
}

#ifdef LINUX
int main ( int argc, char *argv[] ) {
  char message [ 128 ];
  unsigned char sent_l, sent_r;

  // rotation test
  printf ( "Rotation test\n" );

  mc_set_by_receiver ( 11, 15, &sent_l, &sent_r, message ); // rotate left on spot
  mc_set_by_receiver ( 11, 15, &sent_l, &sent_r, message ); // rotate left on spot
  mc_set_by_receiver ( 11, 15, &sent_l, &sent_r, message ); // rotate left on spot
  mc_set_by_receiver ( 15, 15, &sent_l, &sent_r, message ); // do nothing

  mc_set_by_receiver ( 19, 15, &sent_l, &sent_r, message ); // rotate right on spot
  mc_set_by_receiver ( 19, 15, &sent_l, &sent_r, message ); // rotate right on spot
  mc_set_by_receiver ( 19, 15, &sent_l, &sent_r, message ); // rotate right on spot
  mc_set_by_receiver ( 15, 15, &sent_l, &sent_r, message ); // do nothing

  // forward/back test
  printf ( "Forward/back test\n" );

  mc_set_by_receiver ( 15, 11, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 11, &sent_l, &sent_r, message );

  mc_set_by_receiver ( 15, 15, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 17, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 18, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 19, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 20, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 15, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 15, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 11, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 11, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 11, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 11, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 11, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 11, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 15, 11, &sent_l, &sent_r, message );

  mc_set_by_receiver ( 19, 19, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 19, 19, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 11, 11, &sent_l, &sent_r, message );
  mc_set_by_receiver ( 11, 11, &sent_l, &sent_r, message );

  return ( 0 );
}
#endif
