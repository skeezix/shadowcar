#include <avr/io.h>
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

  /* do something useful..
   */

#if 1
  char *p;

  if ( 1 ) {
    char  *source = "shitfuck";
    unsigned int i;
    for ( i = 0; i < strlen ( source ); i++ ) {
      textbuf [ i ] = source [ i ];
    }
    textbuf [ i ] = '\0';
    lcd_xy( 0, 0 );
    lcd_puts( textbuf );			// display number right adjusted
  }
#endif

  /* churn forever
   */
  while(1);

  return ( 0 );
}
