#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>

#include <Servo.h>

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define WHEEL_SIZE 3
#define TICKS_PER_REV 360

#define ENCODER_LEFT_A 2
#define ENCODER_LEFT_A_INT 1
#define ENCODER_LEFT_B 8

#define ENCODER_RIGHT_A 3
#define ENCODER_RIGHT_A_INT 0
#define ENCODER_RIGHT_B 7


#define drive_kP 5
#define driveStraight_kP -1.35

Servo left_motor;
Servo right_motor;

Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);

double ticksToInches(int ticks)
{
  return (PI*(double)WHEEL_SIZE)*((double)ticks/(double)TICKS_PER_REV);
}


byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 0, 8, 250);

EthernetUDP Udp;

double gyroConstant = 0;

struct RobotInfo_networksafe
{
  int16_t x;
  int16_t y;

  int16_t theta;
  int16_t error;
  int16_t avgDistance;
};

struct RobotInfo
{
  double x;
  double y;

  double thetaRad;
  double theta;
  
  double error;
  int16_t leftDistance;
  int16_t rightDistance;
  double avgDistance;
};

struct RobotUpdate
{
  int8_t closedLoop;

  int8_t gyroCorrect;
  int16_t theta;
  int16_t dist_target;

  int16_t raw_left;
  int16_t raw_right;
  
  int8_t reset;
};

RobotUpdate input;
RobotInfo info;
RobotInfo_networksafe sendable_info;

void updateComm()
{
  RobotInfo_networksafe out;
  memset(&out, 0, sizeof(out));
  
  out.x = floor(info.x);
  out.y = floor(info.y);
  
  out.theta = floor(info.theta);
  out.error = floor(info.error);
  out.avgDistance = floor(info.avgDistance);
  
  int data = Udp.parsePacket();
  if ( data > 0 )
  {
    byte buf[sizeof(input)]; 
    Udp.read(buf, sizeof(input));
    memcpy(&input, &buf, sizeof(input));
    IPAddress remote = Udp.remoteIP();

    Udp.beginPacket(remote, 1218);
    byte* ptr = (byte*)&out;
    for ( int i = 0; i < sizeof(out); i++ )
    {
      Udp.write(*ptr++);
    } 
    Udp.endPacket();
  }
}


void leftTick()
{
  if ( digitalRead(ENCODER_LEFT_B) )
    info.leftDistance++;
  else
    info.leftDistance--;
}

void rightTick()
{
  if ( digitalRead(ENCODER_RIGHT_B) )
    info.rightDistance--;
  else
    info.rightDistance++;
}

void updateSensors()
{
  static unsigned long lastTime = millis();
  unsigned long now = millis();
  double dt = now-lastTime;
  
  if ( dt >= 10 )
  {
    dt /= 1000;
    sensors_event_t event;
    gyro.getEvent(&event);
   Serial.println(event.gyro.z);
    double radSec = event.gyro.z-gyroConstant;
    if ( fabs(radSec) < 0.03 )
     radSec = 0;
    info.thetaRad += radSec*dt;
    info.theta = info.thetaRad*(180/PI);
    lastTime = now;
  }
  info.avgDistance = ticksToInches((info.leftDistance+info.rightDistance)/2);
}

void updatePosition()
{
  static double posLast = info.avgDistance;
  static unsigned long lastTime = millis();
  unsigned long now = millis();

  double dt = now-lastTime;
  if ( dt >= 10 )
  {
    double posChange = info.avgDistance-posLast;
    info.x += posChange * sin(info.thetaRad);
    info.y += posChange * cos(info.thetaRad);
    posLast = info.avgDistance;
    lastTime = now;
  }
}

void updateControlLoops()
{
  int leftOut = input.raw_left;
  int rightOut = input.raw_right;
  if ( input.closedLoop != 0 )
  {
    info.error = input.dist_target-info.avgDistance;
    int out = info.error * drive_kP;
    leftOut = out;
    rightOut = out;
  }
  if ( input.gyroCorrect != 0 )
  {
    int gyroOffset = (info.theta-input.theta)*driveStraight_kP;
    leftOut += gyroOffset;
    rightOut -= gyroOffset;
  }

  input.raw_left = leftOut;
  input.raw_right = rightOut;
}

void updateMotors()
{
  int l = input.raw_left+90;
  int r = input.raw_right+90;

  if ( l > 180 ) l = 180;
  else if ( l < 0 ) l = 0;

  if ( r > 180 ) r = 180;
  else if ( r < 0 ) r = 0;

  r = 180-r;

  left_motor.write(l);
  right_motor.write(r);
}

void setup()
{
  memset(&input, 0, sizeof(input));
  memset(&info, 0, sizeof(info));
  Ethernet.begin(mac,ip);
  Udp.begin(1218);

  pinMode(ENCODER_LEFT_A, INPUT_PULLUP);
  pinMode(ENCODER_LEFT_B, INPUT_PULLUP);

  pinMode(ENCODER_RIGHT_A, INPUT_PULLUP);
  pinMode(ENCODER_RIGHT_B, INPUT_PULLUP); 

  attachInterrupt(ENCODER_LEFT_A_INT, leftTick, RISING);
  attachInterrupt(ENCODER_RIGHT_A_INT, rightTick, RISING);

  Serial.begin(9600);
  

  left_motor.attach(5);
  right_motor.attach(6);

  //gyro.enableAutoRange(true);
  gyro.begin();
  
  delay(500);
}


void loop()
{
  updateComm();
  updateSensors();
  updatePosition();
  updateControlLoops();
  updateMotors();
  Serial.print(info.x);
  Serial.print(" ");
  Serial.print(info.y);
  Serial.print(" ");
  Serial.println(info.theta);
  if ( input.reset != 0 )
  {
    memset(&input, 0, sizeof(input));
    memset(&info, 0, sizeof(info));
  }
}

