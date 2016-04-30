
#include <SPI.h>
#include <printf.h>
#include "RF24.h"

// Radio pins:
#define pinCE 53  
#define pinCSN 48 



RF24 radio(pinCE,pinCSN);

byte addresses[][7] = {"RoyBot","Remote"};

struct messageStruct
{
  long timeus=0;
  int jlx=0;
  int jly=0;
  int jrx=0;
  int jry=0;
  int bl=0;
  int br=0;
} message;

static void setupRemote()
{
  printf_begin();
  
  radio.begin();

  radio.setPALevel(RF24_PA_MAX);
  //radio.setAutoAck(true);
  radio.setRetries(0,1);
  //radio.enableAckPayload();
  radio.enableDynamicPayloads();
  
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1,addresses[0]);
  
  // Start the radio listening for data
  radio.startListening();
  
  radio.printDetails();
}



static inline bool loopRemote()
{
  
  if( radio.available())
  {       
    radio.read( &message, sizeof(message) );


    //radio.stopListening();
    //radio.write(&telmessage,sizeof(telmessage));
    //radio.startListening(); 
 
    /*Serial.print(message.timeus);
    Serial.print(",");
    Serial.print(message.forward);
    Serial.print(",");
    Serial.println(message.turn);/**/

    float jforward = message.jry*0.001953125;
    float jturn = message.jrx*0.001953125;
    
    forwardspeed = 0.9*forwardspeed + 0.1* jforward * 0.25;
    turnspeed = 0.9*turnspeed + 0.1* jturn * 0.25;


    panpos = 0.5*panpos + 0.5*(message.jlx+512)*0.0009765625;
    tiltpos = 0.5*tiltpos + 0.5*(message.jly+512)*0.0009765625;
    

    /*if(message.br>0)
    {
      // breaking:
      targetx=encoderCountLeft/2+encoderCountRight/2;
      targetdiffx = encoderCountRight-encoderCountLeft;
      forwardspeed = 0;
      turnspeed = 0;
    }/**/

    /*float total = max(abs(forwardspeed+turnspeed),abs(forwardspeed-turnspeed));
    if(total>0.25)
    {
      float factor = 0.25/total;
      forwardspeed = factor*forwardspeed;
      turnspeed =  factor*turnspeed;
    }/**/
  }
  
  
  return false;
}


