#ifndef _PID_
#define _PID_
#include <stdbool.h>


void PID_init(void);
// Methods - double
double PID_compute(double input);

// Methods - void

void PID_tune(double _Kp, double _Ki, double _Kd);
void PID_limit(double min, double max);
void PID_set_point(double newSetpoint);
void PID_set_divisor(double newMinimize);

// Methods - double, getters
double PID_getOutput();



#endif /*_PID_*/
