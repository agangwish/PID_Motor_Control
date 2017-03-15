/* Alex Gangwish
ME 333 Final Project
PID Motor Control */

#include "NU32.h"
#include "encoder.h"
#include "utilities.h"
#include "currentcontrol.h"
#include <stdio.h>
// include other header files here

#define BUF_SIZE 200

int main() {
  char buffer[BUF_SIZE];
  NU32_Startup();
  set_mode(IDLE);
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;
  __builtin_disable_interrupts();
  // in future, intitialize modules or peripherals here
  encoder_init();
  ADC_init();
  currentcontrol_init();
  encoder_reset();
  __builtin_enable_interrupts();

  while(1) {
    NU32_ReadUART3(buffer, BUF_SIZE);
    NU32_LED2 = 1;  // clear the error LED
    switch(buffer[0]) {
      case 'a': {
        // Read current sensor (ADC counts)
        sprintf(buffer, "%d\r\n", get_ADC_counts());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'b': {
        // Read current sensor (milliamps)
        sprintf(buffer, "%d\r\n", get_ADC_milliamps());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'c': {
        // Read encoder (counts)
        sprintf(buffer, "%d\r\n", encoder_ticks());
        NU32_WriteUART3(buffer);                  // send encoder count to client
        break;
      }
      case 'd': {
        // Read encoder (degrees)
        sprintf(buffer, "%d\r\n", encoder_angle());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'e': {
        // reset encoder count to 32,768
        encoder_reset();
        break;
      }
      case 'f': {
        // set PWM between -100 and 100
        int x = 0;
        NU32_ReadUART3(buffer, BUF_SIZE);
        sscanf(buffer, "%d", &x);
        set_duty_cycle(x);
        set_mode(PWM);
        break;
      }
      case 'g': {
        // set current gains
        float kptemp = 0, kitemp = 0;
        NU32_ReadUART3(buffer, BUF_SIZE);
        sscanf(buffer, "%f %f", &kptemp, &kitemp);
        set_current_gains(kptemp, kitemp);
        break;
      }
      case 'h': {
        // get current gains
        float kptemp = 0, kitemp = 0;
        get_current_gains(&kptemp, &kitemp);
        sprintf(buffer, "%f %f\r\n", kptemp, kitemp);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'k': {
        // test current gains
        int i = 0;
        start_test();
        while (!test_finished()) {} // wait for test to finish

        sprintf(buffer, "%d\r\n", PLOTPTS);
        NU32_WriteUART3(buffer);
        for (i = 0; i < PLOTPTS; i++) {
          sprintf(buffer, "%d %d\r\n", get_reference_array(i), get_sensor_array(i));
          NU32_WriteUART3(buffer);
        }
        break;
      }
      case 'p': {
        // Unpower the moter
        set_mode(IDLE);
        set_duty_cycle(0);
        set_direction(0);
        break;
      }
      case 'q': {
        // handle q for quit. Later you may want to return to IDLE mode here
        set_mode(IDLE);
        break;
      }
      case 'r': {
        // get mode
        sprintf(buffer, "%d\r\n", get_mode());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'x': {
        // add two integers and return to client
        int x = 0, y = 0;
        NU32_ReadUART3(buffer, BUF_SIZE);
        sscanf(buffer, "%d %d", &x, &y);  // read numbers into x and y
        sprintf(buffer, "%d\r\n", x + y); // return x + y
        NU32_WriteUART3(buffer);
        break;
      }
      default: {
        NU32_LED2 = 0;  // turn on error LED
        break;
      }
    }
  }
  return 0;
}
