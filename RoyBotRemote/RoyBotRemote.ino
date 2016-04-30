


/*
* DuoBotRemote
* 
* Author: Loic Royer
*/

#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <printf.h>
#include "RF24.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
RF24 radio(8,7);


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

struct telmessageStruct
{
  float data[4];
} telmessage;

void setup() 
{
  //pinMode(13, OUTPUT);
  //digitalWrite(13, LOW); 

  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("DuoBot"));
  lcd.setCursor(0, 1);
  lcd.print(F("Remote Control"));
  
  delay(500);
  Serial.begin(250000);
  Serial.flush();
  //printf_begin();
  //Serial.println(F("DuoBot Remote"));

  delay(1000);
  radio.begin();
  delay(1000);
  radio.setPALevel(RF24_PA_MAX);
  //radio.setAutoAck(true);
  radio.setRetries(0,1);
  //radio.enableAckPayload(); 
  radio.enableDynamicPayloads();

  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[1]);


  //radio.printDetails(); 
}

long count=0;


void loop() 
{
  message.timeus = micros();
  message.jlx = analogRead(A0)-512-5;
  message.jly = analogRead(A1)-512+2;
  message.jrx = analogRead(A2)-512+4;
  message.jry = analogRead(A3)-512-27;
  message.bl = digitalRead(2);
  message.br = digitalRead(3);
    
  /*Serial.print(axisLX);
  Serial.print(",");
  Serial.print(axisLY);
  Serial.print(",");
  Serial.print(axisRX);
  Serial.print(",");
  Serial.print(axisRY);
  Serial.print(",");
  Serial.print(buttonL);
  Serial.print(",");
  Serial.println(buttonR);/**//**/

  /*Serial.print(message.jrx);
  Serial.print(",");
  Serial.println(message.jry);/**/
  

  //digitalWrite(13, LOW);
  //Serial.print(count);
  //Serial.print(F(","));
  if(!radio.write(&message, sizeof(message)))
  {
    //Serial.print(F("0"));
  }
  else
  {
    //Serial.print(F("1"));
  }
  //Serial.print(F(","));

  radio.startListening();
  delay(1);
  if(radio.available())
  {
    //Serial.println(F("AVAILABLE ACK PAYLOAD"));
    radio.read( &telmessage, sizeof(telmessage) );
  }
  radio.stopListening();

  Serial.print(telmessage.data[1]*0.001);
  Serial.print(F(","));
  Serial.print(telmessage.data[2]);
  //Serial.print(F(","));
  //Serial.print(telmessage.thetadot*0.01);
  //Serial.print(F(","));
  //Serial.print(telmessage.data);
  //Serial.print(F(","));
  //Serial.print(telmessage.forward);
  //Serial.print(F(","));
  //Serial.print(telmessage.turn);

  Serial.print(F("\n"));
    
  //digitalWrite(13, HIGH); 


  
  count = (count+1) % 128;  
}






