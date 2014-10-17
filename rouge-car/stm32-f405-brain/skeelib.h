#ifndef h_skeelib_h
#define h_skeelib_h

// string
char* lame_itoa ( int i, char b[] );
uint16_t lame_strlen ( char *p );
void *lame_memcpy ( unsigned char *dest, unsigned char *src, unsigned int n );

// delay time
void lame_delay_ms ( const unsigned int ms );

#endif
