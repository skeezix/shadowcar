#ifndef _main_h_
#define _main_h_

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// avoid push in main
int main( void )        __attribute__((OS_main));

#include "avrutil.h"

#define F_CPU   20000000UL // 20MHz
#include <util/delay.h>

#define	LCD_D4		SBIT( PORTC, 2 )
#define	LCD_DDR_D4	SBIT( DDRC, 2 )

#define	LCD_D5		SBIT( PORTC, 3 )
#define	LCD_DDR_D5	SBIT( DDRC, 3 )

#define	LCD_D6		SBIT( PORTC, 4 )
#define	LCD_DDR_D6	SBIT( DDRC, 4 )

#define	LCD_D7		SBIT( PORTC, 5 )
#define	LCD_DDR_D7	SBIT( DDRC, 5 )

#define	LCD_RS		SBIT( PORTC, 0 )
#define	LCD_DDR_RS	SBIT( DDRC, 0 )

#define	LCD_E0		SBIT( PORTC, 1 )
#define	LCD_DDR_E0	SBIT( DDRC, 1 )

typedef enum {
  el_nil = 0,
  el_x = 1,
  el_y = 2,
} elevon_e;

#endif
