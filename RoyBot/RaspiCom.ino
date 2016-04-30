
#define pinLevelGND 16
#define pinLevelVCC 17


static void setupRaspiCom()
{
  pinMode(pinLevelGND, OUTPUT);
  pinMode(pinLevelVCC, OUTPUT);
  digitalWrite(pinLevelGND, LOW);
  digitalWrite(pinLevelVCC, HIGH);

  Serial3.begin(115200);
  Serial3.println("% DuoBot Here!");
}

int debugcounter=0;


int readInt()
{
  char tmpbuffer[2];

  int nb = Serial3.readBytes(tmpbuffer, 2);
  
  if(nb==2)
  {
    int x = tmpbuffer[0]+tmpbuffer[1]<<8;
    return x;
  }

  return 0;
}

void waitForSync()
{
  while(Serial3.read() != '\n')
  {}  
}


static inline bool loopRaspiCom()
{
  /*if(((debugcounter++)%10) == 0)
  {
    Serial3.print(debugcounter);
    Serial3.println("-DuoBot Here!");
  }/**/

  //Serial3.println("!");

  if(Serial3.available())
  {
    waitForSync();
    int forward = readInt();
    int turn = readInt();
    int pan = readInt();
    int tilt = readInt();

    

    Serial3.println(tilt);
  }
}

