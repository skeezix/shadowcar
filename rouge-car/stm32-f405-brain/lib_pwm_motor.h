
#ifndef h_lib_pwm_motor_h
#define h_lib_pwm_motor_h

// timer is for enable on pc8/pc9
//
// period -> the total width
// pulse -> how long the pulse is within the period (pulse <= period)
void pwm_init_tim8 ( unsigned int period );
void pwm_init_pc8_tim8_ch3 ( unsigned int pulse );
void pwm_init_pc9_tim8_ch4 ( unsigned int pulse );

// steerage is on PA14/15 + PC10/11
void pwm_init_steerage ( void );

// doit!
void pwm_set_left_direction ( signed char dir ); // +1 fwd, -1 backwards, 0 neither
void pwm_set_left_speed ( unsigned char speed ); // 0..255
void pwm_set_right_direction ( signed char dir ); // +1 fwd, -1 backwards, 0 neither
void pwm_set_right_speed ( unsigned char speed ); // 0..255

// test loop
int pwm_test ( void );

#endif
