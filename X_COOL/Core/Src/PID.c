#include "PID.h"
#include "stm32l4xx_hal.h"
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
// Variables - double
double output;
double lastErr;
double errSum;

// Variables - long
unsigned long lastTime;

// Variables - bool
bool doConstrain;
bool init;

// Variables - double - tuining
double Kp;
double Ki;
double Kd;
double divisor;
double minOut;
double maxOut;
double setPoint;
// Variables - long
unsigned long lastTime;

// Variables - double
double output;
double lastErr;
double timeChanged;

// Variables - double, error variables
double error;
double errSum;
double dErr;

// Variables - bool
bool doLimit;
bool init;



void PID_init(void)
{
  Kp = 1;
  Ki = 1;
  Kd = 1;
  divisor = 10;
  doLimit = false;
  init = true;
}

void PID_set_point (double newSetpoint)
{
  setPoint = newSetpoint;
}

void PID_tune (double _Kp, double _Ki, double _Kd)
{
  if (_Kp < 0 || _Ki < 0 || _Kd < 0) return;
  Kp = _Kp;
  Ki = _Ki;
  Kd = _Kd;
}

void PID_limit(double min, double max)
{
  minOut = min;
  maxOut = max;
  doLimit = true;
}



void PID_set_divisor (double newMinimize)
{
  divisor = newMinimize;
}

// Getters
double PID_getOutput(void)
{
  return output;
}


double PID_compute (double sensor)
{
  // Return false if it could not execute;
  // This is the actual PID algorithm executed every loop();

  // Failsafe, return if the begin() method hasn't been called
  if (!init) return 0;

  // Calculate time difference since last time executed
  unsigned long now = HAL_GetTick();
  double timeChange = (double)(now - lastTime);

  // Calculate error (P, I and D)
  double error = setPoint - sensor;
  errSum += error * timeChange;
  if (doLimit) {
    errSum = constrain(errSum, minOut * 1.1, maxOut * 1.1); 
  }
  double dErr = (error - lastErr) / timeChange;

  // Calculate the new output by adding all three elements together
  double newOutput = (Kp * error + Ki * errSum + Kd * dErr) / divisor;

  // If limit is specifyed, limit the output
  if (doLimit) {
    output = constrain(newOutput, minOut, maxOut);
  } else {
    output = newOutput;  
  }

  // Update lastErr and lastTime to current values for use in next execution
  lastErr = error;
  lastTime = now;

  // Return the current output
  return output;
}
