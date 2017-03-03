/* Alex Gangwish
ME 333 Final Project
PID Motor Control */

#include "NU32.h"
#include "encoder.h"
#include <stdio.h>
// include other header files here

#define BUF_SIZE 200

int main() {
  char buffer[BUF_SIZE];
  NU32_Startup();
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;
  __builtin_disable_interrupts();
  // in future, intitialize modules or peripherals here
  encoder_init();
  __builtin_enable_interrupts();

  while(1) {
    NU32_ReadUART3(buffer, BUF_SIZE);
    NU32_LED2 = 1;  // clear the error LED
    switch(buffer[0]) {
      case 'c': {
        // Read encoder (counts)
        sprintf(buffer, "%d", encoder_ticks());
        NU32_WriteUART3(buffer);                  // send encoder count to client
        break;
      }
      case 'd': {
        // increment an integer and return to client
        int n = 0;
        NU32_ReadUART3(buffer, BUF_SIZE);
        sscanf(buffer, "%d", &n);
        sprintf(buffer, "%d\r\n", n + 1); // return the number + 1
        NU32_WriteUART3(buffer);
        break;
      }
      case 'q': {
        // handle q for quit. Later you may want to return to IDLE mode here
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
