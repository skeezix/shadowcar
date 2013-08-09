#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include <avr/io.h>
#include <avr/interrupt.h>   // include interrupt support
#include <util/setbaud.h>
#include <avr/sleep.h>

#include "lcd_drv.h"
#include "mc_sabertooth.h"
#include "TWI_slave.h"

// adafruit lcd wiring tutorial: http://learn.adafruit.com/character-lcds/wiring-a-character-lcd
// install path: /usr/lib/avr/include/avr

// globals: timer
volatile unsigned int g_timer_us = 0; // will overflow and cycle after 6 seconds
volatile unsigned int g_time_s = 0;   // will overflow after 16 hours
volatile unsigned int _g_time_us_tick = 0; // internal ticker to count seconds passage; ms accrue since last sec bump

// globals: receiver
volatile unsigned char g_ch1_duration = 0; // us that last pwm was at (11-19/20, 15 is at rest) 11left<->right   ... 19,19 is top right
volatile unsigned char g_ch2_duration = 0; // us that last pwm was at (11-19/20, 15 is at rest) up19<->down11    ... 11,11 is bottom left

// TWI / I2C commands
unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg );
typedef enum {
  tc_nil = 0,
  tc_heartbeat = 1,
  tc_gethello,
  // 1,2,3,...
} twi_cmd_e;

int main( void )
{
  uint32_t val = 0;
  char textbuf [ (2*16) + 1 ]; // lcd

  /* setup
   */

  // LCD
  lcd_init();

  // Timer: enable a timer so we can measure passage of time
  //
  // Given: 20MHz clock
  // --> if we want resolution of ms (1/1000th second) .. actually, we want us (1/10th of a ms) so we can measure partial ms
  // --> and we have 1/20000000 clock resolution
  // -----> 2000 ticks will get us there (20,000 will get us ms)
  //
  // Goal: Use CTC interupt mode (CTC -> Clear on Timer Compare)
  // So a compare matches, it clears to zero and triggers interupt
  TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode 
  OCR1A = 2000; // number to compare against
  TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt 
  TCCR1B |= (1 << CS10); // Set up timer , with no prescaler (works at full MHz of clock)

  // Receiver setup - set up pin change interupt
#if 1
  EICRA &= ~ ( (1 << ISC01) | (1 << ISC01) ); // clear ISC01+ISC00
  EICRA |= ( (1 << ISC00) ); // 00 set and 01 unset means any edge will make event
  PCMSK0 |= ( (1 << PCINT0) | (1 << PCINT1) ); // Pins to monitor: PA0 and PA1
  PCICR |= (1 << PCIE0); // PA is monitored
#endif

  // Serial - setup (for motor controller)
  mc_setup();

  // TWI - set up
  unsigned char twibuf [ TWI_BUFFER_SIZE ];
  unsigned char TWI_slaveAddress; 
  TWI_slaveAddress = 0x10; // our TWI address
  TWI_Slave_Initialise( (unsigned char)((TWI_slaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_GEN_BIT) )); // Initialise TWI module as slave; include addr+general
  unsigned char twi_heartbeat_counter = 0;

  // setup done - kick up interupts
  sei();

  // Start the TWI transceiver to enable reseption of the first command from the TWI Master.
  TWI_Start_Transceiver();

#if 1 // timer test .. show per-second counter update on lcd
  if ( 1 ) {
    unsigned int last_sec = g_time_s;
    unsigned int last_us = _g_time_us_tick;

    unsigned char sent_l = 0, sent_r = 0;
    char message [ 17 ];

    while(1) {
      unsigned int ch1 = g_ch1_duration;
      unsigned int ch2 = g_ch2_duration;

      // 100ms has past?
      if ( _g_time_us_tick - last_us > 1000 ) {

        mc_set_by_receiver ( ch1, ch2, &sent_l, &sent_r, message );

        last_us = _g_time_us_tick;
      } // .1sec tick

      // one second has past? update lcd
      if ( g_time_s != last_sec ) {
        //sprintf ( textbuf, "recv %2d %2d     ", g_ch1_duration, g_ch2_duration );
        sprintf ( textbuf, "m %2d %2d th %2d %2d #", sent_l, sent_r, ch1, ch2 );
        lcd_xy ( 0, 0 );
        lcd_puts( textbuf ); // display number right adjusted

        //sprintf ( textbuf, "t%2d #", g_time_s );
        sprintf ( textbuf, "t%2d # %s", g_time_s, message );
        lcd_xy ( 0, 1 );
        lcd_puts( textbuf ); // display number right adjusted

        last_sec = g_time_s;
      } // 1 sec tick

      // TWI/I2C stuff, talk to r-pi
      //

      // Check if the TWI Transceiver has completed an operation.
      if ( ! TWI_Transceiver_Busy() ) {

        // Check if the last operation was successful
        if ( TWI_statusReg.lastTransOK ) {

          // Check if the last operation was a reception
          if ( TWI_statusReg.RxDataInBuf ) {
            TWI_Get_Data_From_Transceiver ( twibuf, 2 );

            // Check if the last operation was a reception as General Call        
            if ( TWI_statusReg.genAddressCall ) {
              // don't care

            } else { // Ends up here if the last operation was a reception as Slave Address Match   
              // Example of how to interpret a command and respond.

#if 0            
              // TWI_CMD_MASTER_WRITE stores the data to PORTB
              if (twibuf[0] == TWI_CMD_MASTER_WRITE) {
                PORTB = twibuf[1];                            
              }
#endif

              // TWI_CMD_MASTER_READ prepares the data from PINB in the transceiver buffer for the TWI master to fetch.
              if ( twibuf[0] == tc_heartbeat ) {
                twibuf [ 0 ] = 1;
                twibuf [ 1 ] = twi_heartbeat_counter++;
                TWI_Start_Transceiver_With_Data ( twibuf, TWI_BUFFER_SIZE );
              } else if ( twibuf[0] == tc_gethello ) {
                sprintf ( twibuf + 1, "hello" );
                twibuf [ 0 ] = strlen ( twibuf + 1 ); // len
                TWI_Start_Transceiver_With_Data ( twibuf, TWI_BUFFER_SIZE );
              }

            }

          } else { // Ends up here if the last operation was a transmission  
            // don't care
          }

          // Check if the TWI Transceiver has already been started.
          // If not then restart it to prepare it for new receptions.             
          if ( ! TWI_Transceiver_Busy() ) {
            TWI_Start_Transceiver();
          }

        } else { // Ends up here if the last operation completed unsuccessfully
          TWI_Act_On_Failure_In_Last_Transmission ( TWI_Get_State_Info() );
        } // success/fail

      } // TWI busy?

      // spin
      _delay_ms ( 20 );

    } // while forever

  } // if 1
#endif

  /* churn forever
   */
  while(1);

  return ( 0 );
}

// ISR for timer1 ticking away
ISR(TIMER1_COMPA_vect)
{
  // bump ms timer counter
  g_timer_us++;

  // bump 'seconds' counter as well
  _g_time_us_tick++;
  if ( _g_time_us_tick > 10000 ) {
    _g_time_us_tick = 0;
    g_time_s ++;
  }

} // ISR for timer1

// ISR for pin change events
volatile unsigned int g_ch1_rose = 0; // time that channel1 rose
volatile unsigned int g_ch2_rose = 0; // time that channel1 rose
volatile unsigned char _g_pin_state = 0; // so we can know which pins changed since last interupt
ISR(PCINT0_vect)
{
  unsigned int timer = g_timer_us;
  unsigned char delta = _g_pin_state ^ PINA;

  if ( delta & ( 1 << PA0 ) ) {

    // CH1: edge has gone up, or down?
    if ( PINA & ( 1 << PA0 ) ) {
      // edge has gone high
      g_ch1_rose = timer;
    } else {
      // handle overflow too..
      if ( g_ch1_rose < timer ) {
        g_ch1_duration = timer - g_ch1_rose;
      } else {
        // overflow.. ignore result, who cares if we miss one once in awhile
      }
    }

  } // PA0 changed?

  // COPY PASTE from above, changing channel
  if ( delta & ( 1 << PA1 ) ) {

    // CH2: edge has gone up, or down?
    if ( PINA & ( 1 << PA1 ) ) {
      // edge has gone high
      g_ch2_rose = timer;
    } else {
      // handle overflow too..
      if ( g_ch2_rose < timer ) {
        g_ch2_duration = timer - g_ch2_rose;
      } else {
        // overflow.. ignore result, who cares if we miss one once in awhile
      }
    }

  } // PA1 changed?

  // update last state
  _g_pin_state = PINA;
}

unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg ) {
  // A failure has occurred, use TWIerrorMsg to determine the nature of the failure
  // and take appropriate actions.
  // See header file for a list of possible failures messages.
  
  TWI_Start_Transceiver();
                    
  return ( TWIerrorMsg ); 
}
