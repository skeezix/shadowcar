#ifndef h_mc_goal_h
#define h_mc_goal_h

// using 'goal' based throttling allows for fast moves on stick to be handled more gracefully
// by vehicle; jump from full reverse to full forward, and allow rules to determine (say) if
// it does that, or if it instead graduallty steps down, or if it reverse one motor but not
// another, and so on... -- perhaps saves motor life?

typedef struct {

  unsigned char left_dir;
  unsigned char left_goal_speed;

  unsigned char right_dir;
  unsigned char right_goal_speed;

} mc_value_t;

extern mc_value_t _mc_goal;
extern mc_value_t _mc_current;
#define DEFAULT_SPEED_STEP 10
extern unsigned char mc_max_speed_step = DEFAULT_SPEED_STEP;

#define IS_REVERSAL_ALLOWED 1 /* instead of just deccelerate one side, may reverse it */

#endif
