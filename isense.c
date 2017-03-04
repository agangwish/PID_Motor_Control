#include <xc.h>
#include "isense.h"

#define SAMPLE_TIME 10  // 10 core timer ticks = 250 ns

void ADC_init() {
  AD1PCFGbits.PCFG0 = 0;  // Configure B0 as input
  AD1CON3bits.ADCS = 2;
  AD1CON1bits.ADON = 1;   // turn on ADC
}

unsigned int get_ADC_counts() {
  // return the ADC reading at B0 in counts
  unsigned int elapsed = 0, finish_time = 0;
  AD1CHSbits.CH0SA = 0;   // sample B0
  AD1CON1bits.SAMP = 1;
  elapsed = _CP0_GET_COUNT();
  finish_time = elapsed + SAMPLE_TIME;

  while (_CP0_GET_COUNT() < finish_time) {
    ; // sample at least 250 ns
  }
  AD1CON1bits.SAMP = 0;

  while (!AD1CON1bits.DONE) {
    ; // wait for the conversion process to finish
  }
  return ADC1BUF0;
}

int get_ADC_milliamps() {
  // return the ADC reading at B0 in milliamps
  return (1.5576 * get_ADC_counts()) - 787.0626;  // from best fit line for ADC/I mapping
}
