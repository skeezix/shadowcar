#ifndef _main_h_
#define _main_h_

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "mydefs.h"

#define	XTAL	20000000UL			// 20MHz

#define F_CPU   XTAL
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


#endif
