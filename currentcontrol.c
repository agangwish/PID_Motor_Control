#include <xc.h>
#include "NU32.h"
#include "utilities.h"
#include "currentcontrol.h"
#include "isense.h"
#include "LCD.h"
#include <stdio.h>

static volatile float current_duty_cycle = 0; // range 0 - 100;
static volatile int current_direction = 0;  // 0 = counterclockwise, 1 = clockwise
static volatile float Ki = 0, Kp = 0;
static volatile float eint = 0;
static volatile int square_wave_counter = 0;
static volatile int reference_current = 200;
static volatile int reference_array[PLOTPTS];
static volatile int sensor_array[PLOTPTS];
static volatile int test_is_finished = 0;
static volatile int demanded_current = 0;
static volatile float eintmax = 100;

void currentcontrol_init() {
  /* Initialize Timer 2 Interrupt */
  PR2 = 15999;          // 5 kHz interrupt
  TMR2 = 0;
  T2CONbits.TGATE = 0;  // not gated input (default)
  T2CONbits.ON = 1;     // turn on Timer 2
  IPC2bits.T2IP = 5;    // set interrupt priority
  IPC2bits.T2IS = 0;    // set interrupt subpriority
  IFS0bits.T2IF = 0;    // clear interrupt flag
  IEC0bits.T2IE = 1;    // enable interrupt

  /* Initialize 20 kHz PWM signal */
  PR3 = 3999;             // period = (PR3+1) * N * 12.5ns = 50us = 20khz
  TMR3 = 0;
  OC1CONbits.OCM = 0b110; // PWM mode with fault pin disabled
  OC1CONbits.OCTSEL = 1;  // use Timer 3 for OC1
  OC1RS = 3000;           // duty cycle = OC1RS / (PR3 + 1)
  OC1R = 3000;            // initialize this before turning OC1 on, afterwards it is read-only
  T3CONbits.ON = 1;       // turn on Timer 3
  OC1CONbits.ON = 1;      // turn on OC1

  /* Intialize digital output for motor direction */
  TRISDbits.TRISD10 = 0;  // set D10 to be an output pin
  LATDbits.LATD10 = 0;    // initialize output to low
}

void set_duty_cycle(float cycle) {
  if (cycle > 100) {
    current_duty_cycle = 100;
  } else if (cycle < -100) {
    current_duty_cycle = -100;
  } else {
    current_duty_cycle = abs(cycle);
  }
  if (cycle >= 0) {
    set_direction(0);
  } else {
    set_direction(1);
  }
}

int get_duty_cycle() {
  return current_duty_cycle;
}

void set_direction(int dir) {
  current_direction = dir;
}

int get_direction() {
  return current_direction;
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) current_control_ISR(void) {
  switch(get_mode()) {
    case IDLE: {
      OC1RS = 0; // set duty cycle to 0 (braking)
      break;
    }
    case PWM : {
      OC1RS = ((PR3 + 1) * (get_duty_cycle())) / 100;
      LATDbits.LATD10 = get_direction();  // 0 = counterclockwise, 1 = clockwise
      break;
    }
    case ITEST: {
      test_is_finished = 0;
      int sensed_current = get_ADC_milliamps();
      sensor_array[square_wave_counter] = sensed_current;
      reference_array[square_wave_counter] = reference_current;
      current_PI_controller(sensed_current, reference_current);
      OC1RS = ((PR3 + 1) * (get_duty_cycle())) / 100;
      LATDbits.LATD10 = get_direction();

      if (square_wave_counter == 25 || square_wave_counter == 50 || square_wave_counter == 75) {
        reference_current = -reference_current;
      }
      if (square_wave_counter == 99) {
        test_is_finished = 1;
        set_mode(IDLE);
        square_wave_counter = 0;
        eint = 0;         // reset between runs
      }
      square_wave_counter++;
      break;
    }
    case HOLD: {
      int sensed_current = get_ADC_milliamps();
      current_PI_controller(sensed_current, demanded_current);
      OC1RS = ((PR3 + 1) * (get_duty_cycle())) / 100;
      LATDbits.LATD10 = get_direction();
      break;
    }
    case TRACK: {
      int sensed_current = get_ADC_milliamps();
      current_PI_controller(sensed_current, demanded_current);
      OC1RS = ((PR3 + 1) * (get_duty_cycle())) / 100;
      LATDbits.LATD10 = get_direction();
      break;
    }
    default: {
      break;
    }
  }
  IFS0bits.T2IF = 0;        // clear interrupt flag
}

void set_current_gains(float kp, float ki) {
  __builtin_disable_interrupts();
  Kp = kp;
  Ki = ki;
  eintmax = 100 / Ki;
  __builtin_enable_interrupts();
}
void get_current_gains(float *kp, float *ki) {
  *kp = Kp;
  *ki = Ki;
}

void set_demanded_current(int current) {
  demanded_current = current;
}

int get_demanded_current() {
  return demanded_current;
}

void current_PI_controller(int sensor_val, int reference_val) {
  float e = reference_val - sensor_val;
  eint = eint + e;
  if (eint > eintmax) {
    eint = eintmax;
  } else if (eint < -eintmax) {
    eint = -eintmax;
  }
  float u = Kp * e + Ki * eint;
  set_duty_cycle(u);
}

int test_finished() {
  return test_is_finished;
}

int get_sensor_array(int index) {
  return sensor_array[index];
}

int get_reference_array(int index) {
  return reference_array[index];
}

void start_test() {
  test_is_finished = 0;
  set_mode(ITEST);
}
