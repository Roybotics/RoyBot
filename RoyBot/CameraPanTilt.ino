#include <Servo.h>

// fix jitter with:
// http://scolton.blogspot.de/2010/07/arduino-interrupt-driven-servo-routine.html

Servo pan; 
Servo tilt; 

#define pinPan 6
#define pinTilt 12


#define smin 800
#define smax 2200

static inline void setupCameraPanTilt()
{
 pan.attach(pinPan); 
 tilt.attach(pinTilt); 
}


static inline void loopCameraPanTilt()
{
  pan.writeMicroseconds((int)(smin + panpos*(smax-smin))); 
  tilt.writeMicroseconds((int)(smin + tiltpos*(smax-smin))); 
}
