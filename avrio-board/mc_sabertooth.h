#ifndef h_mc_sabertooth_h
#define h_mc_sabertooth_h

unsigned char mc_setup ( void );

// simple serial interface
typedef enum {
  mcm_nil = 0,
  mcm_left = 1,
  mcm_right = 2,
  mcm_both = 3,
  mcm_allstop = 4,      // special: if this is specified in select, strength is ignored and is set to 'all stop'
} mc_motor_select_e;
// strength:
// <63 is one dir, >63 is other
// 63 -> off
void mc_speed ( mc_motor_select_e select, unsigned char strength ); // strength is 1..127 always

void mc_set_by_receiver ( unsigned char x, unsigned char y ); // x,y are both 11..19 (possibly 10..20 for blips)

inline unsigned char mc_is_deadspace ( unsigned char i ) {
  if ( i > 13 && i < 17 ) {
    return ( 1 );
  }
  return ( 0 );
}

// packetized serial interface

#endif
