
float integralError=0;
float errorPositionLast=0;
float errorAngleLast=0;
float averagePositionError=0;
float attenuation = 1;

static inline void loopBalancing()
{
  noInterrupts();
  const float x = encoderCountLeft/2+encoderCountRight/2;
  const float diffx = encoderCountRight-encoderCountLeft;
  interrupts();

  if(abs(theta)>0.9)
  {
    integralError=0;
    attenuation=1;
    forward=0;
    turn=0;
    return;
  }

  const float errorAngle      =  -3*((float)thetadot) + -(2250*((float)theta));
  const float errorPosition   =  ((float)x-(float)targetx);
  averagePositionError = 0.999*averagePositionError+0.001*errorPosition;

  if(abs(errorPosition-averagePositionError)>100)
  {
    targetx = 0.99*targetx + 0.01*x;
  }

  const float differentialAngleError = 0.5*(errorAngle-errorAngleLast);
  errorAngleLast = errorAngle;

  const float differentialPositionError = 40*(errorPosition-errorPositionLast); 
  errorPositionLast = errorPosition;

  integralError = 0.999*integralError + errorAngle + errorPosition + differentialPositionError + differentialAngleError;

  const float controlForward =    errorAngle
                                + errorPosition
                                + 0.07*integralError
                                + differentialPositionError
                                + differentialAngleError;

  const float turnError = (float)diffx-(float)targetdiffx;
  if(abs(turnError)>30)
  {
    targetdiffx = 0.95*targetdiffx + 0.05*diffx;
  }
  
  const float controlTurn  = 5*turnError;

  if(osc>700)
  {
    attenuation*=0.5;
    forward *= 0.1;
    turn    *= 0.1;
  }
  else if(osc>400)
  {
    attenuation=0.9;
    forward *= 0.9;
    turn    *= 0.9;
  }
  else if(osc<=100)
  {
    attenuation = 0.01*attenuation+0.99*1;
  } 
  else if(osc<=400)
  {
    attenuation = 0.10*attenuation+0.9*1;
  }

 

  telmessage.data[1]    = osc;
  telmessage.data[2]    = attenuation;

  forward= attenuation*controlForward;
  turn = attenuation*controlTurn;

  //Serial.print(turnError);
  //Serial.print(",");
  //Serial.println(errorPosition);
  //Serial.print(osc);
  //Serial.print(",");
  //Serial.println(200*attenuation);
  
}

static inline int8_t sgn(float val) 
{
 if (val < 0) return -1;
 if (val==0) return 0;
 return 1;
}
