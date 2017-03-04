#include <xc.h>
#include "currentcontrol.h"

static volatile int current_duty_cycle = 0; // range 0 - 100;
static volatile int current_direction = 0;  // 0 = counterclockwise, 1 = clockwise

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

void set_duty_cycle(int cycle) {
  current_duty_cycle = cycle;
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
