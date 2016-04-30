

/*
* RoyBot
* 
* Author: Loic Royer
*/

// Local includes:
#include <digitalWriteFast.h>

void setup() 
{
  // Initialize UART Serial
  Serial.begin(250000);

  setupRemote();
  setupMPU();
  setupMotors();
  setupCameraPanTilt();
  setupRaspiCom();
  
}

// Encoder 
volatile long encoderCountLeft=0, encoderCountRight=0;

// IMU:
volatile float theta,thetadot;
volatile float osc;

// 
volatile float targetx=0, targetdiffx=0;

// Motor forwrad and turn:
volatile int forward=0,turn=0;

// Forward and turn speeds:
float forwardspeed, turnspeed;

// Forward and turn speeds:
float panpos, tiltpos;


// Telemetry:
struct telmessageStruct
{
  float data[4];
} telmessage;


void loop() 
{
  //unsigned long start = micros();  
  
  if(loopMPU())
  {
    // We run the balancing code and motor control only if we have updated data from the IMU.
    loopBalancing();
    loopMotorControl();
    //unsigned long end = micros();
    //unsigned long delta = end - start;
    //telmessage.data[0] = delta*(980.0/(1000*7800.0));
    //Serial.println(delta);
  }
  else
  {
    loopRemote();
    loopCameraPanTilt();
    loopRaspiCom();
  }

  targetx+=forwardspeed;
  targetdiffx+=turnspeed;
  
  //targetx+=0.1;
  //targetdiffx+=0.1;

}
