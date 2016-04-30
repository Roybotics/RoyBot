


#ifndef sbi
  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


// Left Motor pins:
#define pinMRA 23  
#define pinMRB 22 
#define pinMRE 5 

// Right Motor pins:
#define pinMLA 24 
#define pinMLB 25 
#define pinMLE 4  

// Encoder pins:
#define pinERT 19 
#define pinELT 3
#define pinERD 18 
#define pinELD 2



// Setting up motors:
static void setupMotors()
{
  //Initialize motor pins:
  pinMode(pinMLA, OUTPUT);
  pinMode(pinMLB, OUTPUT);
  pinMode(pinMLE, OUTPUT);
    
  pinMode(pinMRA, OUTPUT);
  pinMode(pinMRB, OUTPUT);
  pinMode(pinMRE, OUTPUT);

  pinMode(pinERT, INPUT);
  pinMode(pinERD, INPUT);
  pinMode(pinELT, INPUT);
  pinMode(pinELD, INPUT);

  // Attach interupts:
  attachInterrupt(digitalPinToInterrupt(pinELT), encoderCallBackLeft, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinERT), encoderCallBackRight, FALLING);

  // Set initial pin states:
  digitalWrite(pinMLA, LOW);
  digitalWrite(pinMLB, LOW);
  digitalWrite(pinMLE, HIGH);
  
  digitalWrite(pinMRA, LOW);
  digitalWrite(pinMRB, LOW);
  digitalWrite(pinMRE, HIGH); 

  // Setting up fast PWM:
  TCCR0B &= ~7;   // this operation (AND plus NOT),  set the three bits in TCCR2B to 0
  TCCR0B |= 2;    // sets the prescaler

  TCCR3B &= ~7;   // this operation (AND plus NOT),  set the three bits in TCCR2B to 0
  TCCR3B |= 2;    // sets the prescaler

  analogWrite(pinMLE, 1);
  analogWrite(pinMRE, 1);
}

static void encoderCallBackLeft()
{
  if (digitalReadFast(pinELD))
  {
    encoderCountLeft ++;
  }
  else
  {
    encoderCountLeft --;
  }
}

static void encoderCallBackRight()
{
  if (digitalReadFast(pinERD))
  {
    encoderCountRight --;
  }
  else
  {
    encoderCountRight ++;
  }
}

static inline void setMotor(int speedLeft, int speedRight)
{
  if(speedLeft==0)
  {
    sbi(TCCR0A, COM0B1);
    OCR0B = 0;
    digitalWriteFast(pinMLA, LOW);
    digitalWriteFast(pinMLB, LOW);
  }
  else
  {
    //analogWrite(pinMLE, abs(speedLeft));
    sbi(TCCR0A, COM0B1);
    OCR0B = abs(speedLeft);
    digitalWriteFast(pinMLA, speedLeft>0 ? LOW: HIGH);
    digitalWriteFast(pinMLB, speedLeft>0 ? HIGH: LOW);
  }

  if(speedRight==0)
  {
    sbi(TCCR3A, COM3A1);
    OCR3A = 0;
    digitalWriteFast(pinMRA, LOW);
    digitalWriteFast(pinMRB, LOW);
  }
  else
  {
    //analogWrite(pinMRE, abs(speedRight));
    sbi(TCCR3A, COM3A1);
    OCR3A = abs(speedRight);
    digitalWriteFast(pinMRA, speedRight>0 ? LOW: HIGH);
    digitalWriteFast(pinMRB, speedRight>0 ? HIGH: LOW);
  }
}


static inline void loopMotorControl()
{
  setMotor(constrain(forward+turn,-255,255),constrain(forward-turn,-255,255));
}
