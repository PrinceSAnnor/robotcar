#include "Wire.h"
#include "sensorbar.h"


#define trigger A0
#define echoLeft 2
#define S0 3
#define S2 4
#define S3 5
#define leftPWM 6
#define leftDir 7
#define rightDir 8
#define rightPWM 9
#define OUTRight 10
#define OUTLeft 11
#define echoRight 12
#define echoCenter 13

const uint8_t SX1509_ADDRESS = 0x3E;
SensorBar mySensorBar(SX1509_ADDRESS);

uint8_t state;

//int lRSpeed = 90;
int lRSpeed = 15;
int mSpeed = 20;
int leftFwr = LOW;
int rightFWr = HIGH;
int greenRight = 0;
int blueRight = 0;
int redRight = 0;
int greenLeft = 0;
int blueLeft = 0;
int redLeft = 0;
long durationLeft;
int distanceLeft;
long durationRight;
int distanceRight;
long durationCenter;
int distanceCenter;

#define RIGHT_WHEEL_POL 1
#define LEFT_WHEEL_POL 1
#define IDLE_STATE 0
#define READ_LINE 1
#define GO_FORWARD 2
#define GO_LEFT 3
#define GO_RIGHT 4

void moveLeft()
{
  digitalWrite(rightDir, rightFWr);
  analogWrite(rightPWM, lRSpeed);
  digitalWrite(leftDir, !leftFwr);
  analogWrite(leftPWM, lRSpeed);
  delay(200);
}

void moveFwd()
{
  digitalWrite(rightDir, rightFWr);
  analogWrite(rightPWM, mSpeed);
  digitalWrite(leftDir, leftFwr);
  analogWrite(leftPWM, mSpeed);
}

void moveBck()
{
  digitalWrite(rightDir, !rightFWr);
  analogWrite(rightPWM, mSpeed);
  digitalWrite(leftDir, !leftFwr);
  analogWrite(leftPWM, mSpeed);
}

void moveRight()
{
  digitalWrite(rightDir, !rightFWr);
  analogWrite(rightPWM, lRSpeed);
  digitalWrite(leftDir, leftFwr);
  analogWrite(leftPWM, lRSpeed);
  delay(200);
}

void stopMoving()
{
  digitalWrite(rightDir, rightFWr);
  analogWrite(rightPWM, 0);
  digitalWrite(leftDir, leftFwr);
  analogWrite(leftPWM, 0);
}

void setForGreen(){
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
}
void setForBlue(){
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
}
void setForRed(){
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
}

void setup()
{
  pinMode(rightDir, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(leftDir, OUTPUT);
  pinMode(leftPWM, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(trigger, OUTPUT);
  
  pinMode(echoLeft, INPUT);
  pinMode(echoRight, INPUT);
  pinMode(echoCenter, INPUT);
  pinMode(OUTRight, INPUT);
  pinMode(OUTLeft, INPUT);

  digitalWrite(S0,HIGH);//20% frequency scaling
  
  mySensorBar.setBarStrobe();//only turn on IR during reads
  mySensorBar.clearInvertBits();//Default dark on light
  uint8_t returnStatus = mySensorBar.begin();
  Serial.begin(9600);
  if(returnStatus)
  {
    Serial.println("sx1509 IC communication OK");
  }
  else
  {
    Serial.println("sx1509 IC communication FAILED!");
    while(1);
  }
}

void loop()
{
  /*
  setForGreen();
  greenRight = pulseIn(OUTRight, LOW);
  greenLeft = pulseIn(OUTLeft, LOW);
  Serial.print("GRight:");
  Serial.print(greenRight);
  Serial.print(" GLeft:");
  Serial.println(greenLeft);
  delay(100);
  setForBlue();
  blueRight = pulseIn(OUTRight, LOW);
  blueLeft = pulseIn(OUTLeft, LOW);
  Serial.print("BRight:");
  Serial.print(blueRight);
  Serial.print(" BLeft:");
  Serial.println(blueLeft);
  delay(100);
  setForRed();
  redRight = pulseIn(OUTRight, LOW);
  redLeft = pulseIn(OUTLeft, LOW);
  Serial.print("RRight:");
  Serial.print(redRight);
  Serial.print(" RLeft:");
  Serial.println(redLeft);
  //delay(100);
  */
  uint8_t rawValue = mySensorBar.getRaw();
  Serial.println(rawValue);
  Serial.print("Bin value of input: ");
  for( int i = 6; i >= 0; i-- )
  {
    Serial.print((rawValue >> i) & 0x01);
  }
  Serial.println("b");

  //Print the hex value to the serial buffer.  
  Serial.print("Hex value of bar: 0x");
  if(rawValue < 0x10) //Serial.print( , HEX) doesn't pad zeros. Do it here
  {
    //Pad a 0;
    Serial.print("0");
  }
  Serial.println(rawValue, HEX);
  
  //Print the position and density quantities
  Serial.print("Position (-127 to 127): ");
  int pos = mySensorBar.getPosition();
  Serial.println(pos);
  Serial.print("Density, bits detected (of 8): ");
  int density = mySensorBar.getDensity();
  Serial.println(density);
  Serial.println("");
  //delay(666);
/*
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  durationLeft = pulseIn(echoLeft, HIGH);
  // Calculating the distance
  distanceLeft = durationLeft*0.034/2;

  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  durationRight = pulseIn(echoRight, HIGH);
  // Calculating the distance
  distanceRight = durationRight*0.034/2;

  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  durationCenter = pulseIn(echoCenter, HIGH);
  // Calculating the distance
  distanceCenter = durationCenter*0.034/2;
  // Prints the distance on the Serial Monitor
  Serial.print("DistanceLeft: ");
  Serial.print(distanceLeft);
  Serial.print(" DistanceRight: ");
  Serial.print(distanceRight);
  Serial.print(" DistanceCenter: ");
  Serial.println(distanceCenter);
  */
  //Wait 2/3 of a second
  int range = 25;

  if(pos < range && pos > -range){//go forward
    Serial.println("FORWARD ");
    moveFwd();
    //delay(50);
   }
  else if(pos >= range && pos !=127){//go left
    Serial.print("RIGHT ");
    float strengthRatio = pos / 126.0;
    lRSpeed = 15 + strengthRatio * 60;
    Serial.println(lRSpeed);
    stopMoving();
    moveRight();
   }
  else if(pos <= -range && pos !=-127){//go right
    Serial.print("LEFT ");
    Serial.print(lRSpeed);
    Serial.print(" pos ");
    Serial.print(pos);
    Serial.print(" ratio ");
    double strengthRatio = pos / -126.0;
    Serial.print(strengthRatio);
    lRSpeed = 15 + strengthRatio * 60;
    stopMoving();
    moveLeft();
    
   }
  else{
    moveFwd();
    //moveLeft();
  }
  //stopMoving();
  //delay(666);
}
