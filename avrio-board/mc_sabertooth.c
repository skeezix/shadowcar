#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include <avr/io.h>
#include <avr/interrupt.h>   // include interrupt support
#include <util/setbaud.h>
#include "lcd_drv.h"
#include "mc_sabertooth.h"

unsigned char mc_setup ( void ) {

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

  return;
}

void mc_speed ( mc_motor_select_e select, unsigned char strength ) {

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

  return;
}

void mc_set_by_receiver ( unsigned char x, unsigned char y,
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
