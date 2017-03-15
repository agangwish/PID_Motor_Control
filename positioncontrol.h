#ifndef POSITIONCONTROL__H__
#define POSITIONCONTROL__H__

#define TRAJECTORY_SIZE 2000

void positioncontrol_init();
void set_position_gains(float kp, float ki, float kd);
void get_position_gains(float *kp, float *ki, float *kd);
void set_desired_angle(float angle);
float get_desired_angle();
void PID_controller(int sensor_val, float reference_val);
void reset_position_eint();
void start_trajectory_execution();
int trajectory_finished();
void set_desired_trajectory(int index, float value);
float get_desired_trajectory(int index);
void set_actual_trajectory(int index, float value);
float get_actual_trajectory(int index);

#endif
