#include "positioncontrol.h"
#include "NU32.h"
#include "utilities.h"
#include "encoder.h"
#include "currentcontrol.h"
#include <xc.h>

static volatile float Kp = 0, Ki = 0, Kd = 0;
static volatile float eint = 0, eprev = 0;
static volatile float desired_angle = 0;
static volatile float eintmax = 300;
static volatile int counter = 1;
static volatile int finished = 1;
static volatile float desired_trajectory[TRAJECTORY_SIZE];
static volatile float actual_trajectory[TRAJECTORY_SIZE];

void positioncontrol_init() {
  /* Initialize Timer 4 Interrupt */
  PR4 = 49999;          // 200 Hz interrupt
  TMR4 = 0;
  T4CONbits.TGATE = 0;  // not gated input (default)
  T4CONbits.T32 = 0;    // 16-bit timer
  T4CONbits.TCKPS = 0b011;
  T4CONbits.ON = 1;     // turn on Timer 4
  IPC4bits.T4IP = 5;    // set interrupt priority
  IPC4bits.T4IS = 0;    // set interrupt subpriority
  IFS0bits.T4IF = 0;    // clear interrupt flag
  IEC0bits.T4IE = 1;    // enable interrupt
}

void set_position_gains(float kp, float ki, float kd) {
  __builtin_disable_interrupts();
  Kp = kp;
  Ki = ki;
  Kd = kd;
  __builtin_enable_interrupts();
}

void get_position_gains(float *kp, float *ki, float *kd) {
  *kp = Kp;
  *ki = Ki;
  *kd = Kd;
}

void set_desired_angle(float angle) {
  desired_angle = angle;
}

float get_desired_angle() {
  return desired_angle;
}

void PID_controller(int sensor_val, float reference_val) {
  float e = reference_val - sensor_val;
  eint = eint + e;
  if (eint > eintmax) {
    eint = eintmax;
  } else if (eint < -eintmax) {
    eint = -eintmax;
  }
  float edot = e - eprev;
  float u = Kp * e + Ki * eint + Kd * edot;
  set_demanded_current(u);
  eprev = e;
}

void reset_position_eint() {
  eint = 0;
  eprev = 0;
}

void start_trajectory_execution() {
  finished = 0;
  reset_position_eint();
  counter = 1;
}

int trajectory_finished() {
  return finished;
}

void set_desired_trajectory(int index, float value) {
  desired_trajectory[index] = value;
}

float get_desired_trajectory(int index) {
  return desired_trajectory[index];
}
void set_actual_trajectory(int index, float value) {
  actual_trajectory[index] = value;
}

float get_actual_trajectory(int index) {
  return actual_trajectory[index];
}


void __ISR(_TIMER_4_VECTOR, IPL5SOFT) position_control_ISR(void) {
  switch(get_mode()) {
    case IDLE: {
      break;
    }
    case PWM : {
      break;
    }
    case ITEST: {
      break;
    }
    case HOLD: {
      /* Convert desired angle to 1/10th degrees to match encoder_angle() */
      PID_controller(encoder_angle(), (desired_angle * 10));
      break;
    }
    case TRACK: {
      int n = (int)desired_trajectory[0];
      actual_trajectory[counter] = (float)encoder_angle() / 10;
      /* Convert desired angle to 1/10th degrees to match encoder_angle() */
      PID_controller(encoder_angle(), (desired_trajectory[counter] * 10));
      //desired_angle = desired_trajectory[counter];
      counter++;
      if (counter == n) { // all values read
        desired_angle = desired_trajectory[counter];
        set_mode(IDLE);
        counter = 0;
        finished = 1;
      }
      break;
    }
    default: {
      break;
    }
  }
  IFS0bits.T4IF = 0;        // clear interrupt flag
}
