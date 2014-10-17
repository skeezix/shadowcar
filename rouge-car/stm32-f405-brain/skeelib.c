//#include <eZ80.h>
#include <stm32f4xx.h>
#include <stdio.h>

#include "skeelib.h"

char* lame_itoa ( int i, char b[] ) {
	char const digit[] = "0123456789";
	int shifter;
	char* p = b;
	
	if ( i < 0 ){
		*p++ = '-';
		i *= -1;
	}
	
	shifter = i;
	
	do { //Move to where representation ends
		++p;
		shifter = shifter/10;
	} while ( shifter );
	
	*p = '\0';
	do { //Move back, inserting digits as u go
		*--p = digit[i%10];
		i = i/10;
	} while ( i );

	return ( b );
}

uint16_t lame_strlen ( char *p ) {
	char *e = p;
	
	while ( *e != '\0' ) {
		e++;
	}

	return ( e - p );
}

void *lame_memcpy ( unsigned char *dest, unsigned char *src, unsigned int n ) {
  while ( n ) {
    *dest++ = *src++;
    n--;
  }
  return dest;
}

#define STM32_CLOCK_HZ 120000000UL
#define STM32_CYCLES_PER_LOOP 3 // This will need tweaking or calculating
void lame_delay_ms ( unsigned int ms ) {
  ms *= STM32_CLOCK_HZ / 1000 / STM32_CYCLES_PER_LOOP;

  __asm__ volatile(" mov r0, %[ms] \n\t"
             "1: subs r0, #1 \n\t"
             " bhi 1b \n\t"
               :
               : [ms] "r" (ms)
               : "r0");
}
