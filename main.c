/* Alex Gangwish
ME 333 Final Project
PID Motor Control */

#include "NU32.h"
#include <stdio.h>
// include other header files here

#define BUF_SIZE 200

int main() {
  char buffer[BUF_SIZE];
  char msg[BUF_SIZE];
  NU32_Startup();
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;
  __builtin_disable_interrupts();
  // in future, intitialize modules or peripherals here
  __builtin_enable_interrupts();

  while(1) {
    sprintf(msg, "Enter a letter: ");
    NU32_WriteUART3(msg);
    NU32_ReadUART3(buffer, BUF_SIZE);
    NU32_LED2 = 1;  // clear the error LED
    sprintf(msg, "%c\r\n", buffer[0]);
    NU32_WriteUART3(msg);
    sprintf(msg, "You selected '%c'\r\n", buffer[0]);
    NU32_WriteUART3(msg);
    switch(buffer[0]) {
      case 'd': {
        int n = 0;
        sprintf(msg, "Enter number: ");
        NU32_WriteUART3(msg);
        NU32_ReadUART3(buffer, BUF_SIZE);
        sscanf(buffer, "%d", &n);
        sprintf(msg, "%d\r\n", n);
        NU32_WriteUART3(msg);
        sprintf(msg, "Number + 1: %d\r\n", n + 1); // return the number + 1
        NU32_WriteUART3(msg);
        break;
      }
      case 'q': {
        // handle q for quit. Later you may want to return to IDLE mode here
        sprintf(msg, "You entered the 'quit' case\r\n");
        NU32_WriteUART3(msg);
        break;
      }
      default: {
        NU32_LED2 = 0;  // turn on error LED
        sprintf(msg, "You entered the 'default' case\r\n");
        NU32_WriteUART3(msg);
        break;
      }
    }
  }
  return 0;
}
