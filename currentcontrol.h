#ifndef CURRENTCONTROL__H__
#define CURRENTCONTROL__H__

void currentcontrol_init();
void set_duty_cycle(int cycle);
int get_duty_cycle();
void set_direction(int dir);
int get_direction();
#endif
