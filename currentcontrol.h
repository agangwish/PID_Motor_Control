#ifndef CURRENTCONTROL__H__
#define CURRENTCONTROL__H__

#define PLOTPTS 100
void currentcontrol_init();
void set_duty_cycle(int cycle);
int get_duty_cycle();
void set_direction(int dir);
int get_direction();
void set_current_gains(float kp, float ki);
void get_current_gains(float *kp, float *ki);
void current_PI_controller(int sensor_val, int reference_val);
int test_finished();
int get_sensor_array(int index);
int get_reference_array(int index);
void start_test();
#endif
