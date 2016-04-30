#include <I2Cdev.h>
#include <helper_3dmath.h>
#include <MPU6050_6Axis_MotionApps20.h>

MPU6050 mpu;

// MPU control/status vars
bool MPUReady = false;
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 gyro;
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
float avgarz;


static void setupMPU()
{
  // Initialize i2C
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz). Comment this line if having compilation difficulties with TWBR.

  // initialize mpu:
  Serial.println(F("Initializing MPU6050 device..."));
  mpu.initialize();

  Serial.println(F("Testing device connection..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  uint8_t  devStatus = mpu.dmpInitialize();
  Serial.print(F("status: "));
  Serial.println(devStatus);

  mpu.setDLPFMode(MPU6050_DLPF_BW_98);

  // make sure it worked (returns 0 if so)
  if (devStatus == 0)
  {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready!"));
    MPUReady = true;

    mpu.setXAccelOffset(-9070/8);
    mpu.setYAccelOffset(-29100/8);
    mpu.setZAccelOffset(+602/8+14584/8);
    mpu.setXGyroOffset(59/4);
    mpu.setYGyroOffset(332/4);
    mpu.setZGyroOffset(40/4);

    mpu.setFullScaleGyroRange(3);

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();

    Serial.print(F("Waiting for Correct orientation... "));
    waitForMPU();
    Serial.println(F("Ready!"));
  }
  else
  {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    if (devStatus == 1)
      Serial.print(F("initial memory load failed"));
    else if (devStatus == 2)
      Serial.print(F("DMP configuration updates failed"));

    Serial.println(F(")"));
  }
}

static bool waitForMPU()
{
  while(!loopMPU());
  //Serial.println(theta);
  while(abs(theta)>0.01 && osc>50)
  {
    while(!loopMPU());
    //Serial.println(theta);
  }
}

static inline bool loopMPU()
{
  // if programming failed, don't try to do anything
  if (!MPUReady) return false;

  if ((fifoCount = mpu.getFIFOCount()) < packetSize)
    return false;

  // Get status:
  uint8_t mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count:
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024)
  {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

    return false;
  }
  else if (mpuIntStatus & 0x02)
  {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // Clear any partial data
    mpu.resetFIFO();

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetGyro(&gyro, fifoBuffer);
    theta = gravity.y;
    thetadot = gyro.x;
    avgarz = 0.99*avgarz+0.01*aa.z;
    osc = abs(aa.z-avgarz);

    serialDebugMPU();

    return true;
  }
}



static inline void serialDebugMPU()
{
//#define OUTPUT_READABLE_RAWACCEL

#ifdef OUTPUT_READABLE_GRAVITY
    // display initial world-frame acceleration, adjusted to remove gravity
    // and rotated based on known orientation from quaternion
    //mpu.dmpGetQuaternion(&q, fifoBuffer);
    //mpu.dmpGetGravity(&gravity, &q);
    Serial.print(gravity.x);
    Serial.print(",");
    Serial.print(gravity.y);
    Serial.print(",");
    Serial.println(gravity.z);
#endif

#ifdef OUTPUT_READABLE_GYRO
    // display initial world-frame acceleration, adjusted to remove gravity
    // and rotated based on known orientation from quaternion
    mpu.dmpGetGyro(&gyro, fifoBuffer);
    Serial.print(gyro.x);
    Serial.print(",");
    Serial.print(gyro.y);
    Serial.print(",");
    Serial.println(gyro.z);
#endif


#ifdef OUTPUT_READABLE_QUATERNION
    // display quaternion values in easy matrix form: w x y z
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    Serial.print(q.w);
    Serial.print(",");
    Serial.print(q.x);
    Serial.print(",");
    Serial.print(q.y);
    Serial.print(",");
    Serial.println(q.z);
#endif

#ifdef OUTPUT_READABLE_EULER
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetEuler(euler, &q);
    Serial.print(euler[0] * 180 / M_PI);
    Serial.print(",");
    Serial.print(euler[1] * 180 / M_PI);
    Serial.print(",");
    Serial.println(euler[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_YAWPITCHROLL
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    Serial.print(ypr[0] * 180 / M_PI);
    Serial.print(",");
    Serial.print(ypr[1] * 180 / M_PI);
    Serial.print(",");
    Serial.println(ypr[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_RAWACCEL
    // display real acceleration, adjusted to remove gravity
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    Serial.print(aa.x);
    Serial.print(",");
    Serial.print(aa.y);
    Serial.print(",");
    Serial.println(aa.z);
#endif

#ifdef OUTPUT_READABLE_REALACCEL
    // display real acceleration, adjusted to remove gravity
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    Serial.print(aaReal.x);
    Serial.print(",");
    Serial.print(aaReal.y);
    Serial.print(",");
    Serial.println(aaReal.z);
#endif

#ifdef OUTPUT_READABLE_WORLDACCEL
    // display initial world-frame acceleration, adjusted to remove gravity
    // and rotated based on known orientation from quaternion
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
    mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
    Serial.print(aaWorld.x);
    Serial.print(",");
    Serial.print(aaWorld.y);
    Serial.print(",");
    Serial.println(aaWorld.z);
#endif

#ifdef OUTPUT_TEAPOT
    // display quaternion values in InvenSense Teapot demo format:
    teapotPacket[2] = fifoBuffer[0];
    teapotPacket[3] = fifoBuffer[1];
    teapotPacket[4] = fifoBuffer[4];
    teapotPacket[5] = fifoBuffer[5];
    teapotPacket[6] = fifoBuffer[8];
    teapotPacket[7] = fifoBuffer[9];
    teapotPacket[8] = fifoBuffer[12];
    teapotPacket[9] = fifoBuffer[13];
    Serial.write(teapotPacket, 14);
    teapotPacket[11]++; // packetCount, loops at 0xFF on purpose
#endif
}

