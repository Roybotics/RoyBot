void customDelay(long ms)
{
  for(int i=0; i<ms; i++)
  {
    //Serial.print("encoderCountLeft=");
    Serial.print(encoderCountLeft);
    Serial.print(",");
    Serial.println(encoderCountRight);
    delay(1);
  } 
}
