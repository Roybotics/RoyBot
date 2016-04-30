/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo1;
Servo myservo2; // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

#define smin 800
#define smax 2200

void setup() 
{
  Serial.begin(250000);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }  
  myservo1.attach(4);  
  myservo2.attach(5);  
}

void loop() 
{
  int jlx = analogRead(A0)-512-5;
  int jly = analogRead(A1)-512+2;
  int jrx = analogRead(A2)-512+4;
  int jry = analogRead(A3)-512-27;

  float value = (jly+512)/1024.0;

  int  pos =  (int)(smin + value*(smax-smin));

  Serial.println(pos);

  myservo1.writeMicroseconds(pos); 
  myservo2.writeMicroseconds(pos);  

 /*
  myservo1.writeMicroseconds(smin); 
  myservo2.writeMicroseconds(smin); 
  delay(2000); 

  myservo1.writeMicroseconds((smin+smax)/2); 
  myservo2.writeMicroseconds((smin+smax)/2); 
  delay(2000); 

  myservo1.writeMicroseconds(smax); 
  myservo2.writeMicroseconds(smax); 
  delay(2000); 
  /**/

  /*
  for (pos = smin; pos <= smax; pos += 1) 
  { 
    // in steps of 1 degree
    myservo1.writeMicroseconds(pos); 
    myservo2.writeMicroseconds(pos);     
    delay(1);                     
  }
  for (pos = smax; pos >= smin; pos -= 1) 
  { 
    myservo1.writeMicroseconds(pos); 
    myservo2.writeMicroseconds(pos);             
    delay(1);                       
  }/**/
}

