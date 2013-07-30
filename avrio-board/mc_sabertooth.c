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

  // Y -> set power (full or partial forward or backward)
  // X -> scales this for each motor; full left means no left motor, full right, where full is 'Y'
  unsigned char str_l, str_r;      // determined values
  unsigned char forward;           // temp
  unsigned char strength;          // temp

  // dead space .. if throttle is not high enough in one dir or the other, just all-stop
  if ( mc_is_deadspace ( y ) ) {
    mc_speed ( mcm_allstop, 0 );
    return;
  }

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
  case 2:   strength = 10; break;
  default:  strength = 15;
  }

  // scale strength by X left/rightness
  if ( mc_is_deadspace ( x ) ) {
    // nada .. full power
    
    // convert to actual strength value
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
      strcpy ( r_message, "hdead" );
    }

  } else {
    // apply power differently to each side

    if ( x > DEADCEIL ) {
      // turn right: reduce power on right side, keep left side strong
      str_l = strength;
      str_r = strength - 
        ( ( x - DEADCEIL ) * 5 );
    } else {
      str_r = strength;
      str_l = strength - 
        ( ( DEADFLOOR - x ) * 5 );
    }

    // convert to actual strength value
    if ( forward ) {
      str_l += MOTOROFF;
      str_r += MOTOROFF;
    } else {
      str_l = MOTOROFF - str_l;
      str_r = MOTOROFF - str_r;
    }

    // set speeds
    mc_speed ( mcm_left, str_l );
    mc_speed ( mcm_right, str_r );

    if ( r_sent_l ) {
      *r_sent_l = str_l;
    }
    if ( r_sent_r ) {
      *r_sent_r = str_r;
    }
    if ( r_message ) {
      strcpy ( r_message, "hdif" );
    }

  }

  return;
}
