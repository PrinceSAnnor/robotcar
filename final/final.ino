#include "Wire.h"
#include "sensorbar.h"
#define ENC_COUNT_REV 374

#define trigger A0
//#define echoLeft 2
#define EncR 2 
//#define S0 3
#define EncL 3 
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

uint8_t state=0;

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
int pos = 0;
int density = 0;
float rpm = 0.0;
volatile long encoderValueR = 0;
volatile long encoderValueL = 0;
 
void setup()
{
  pinMode(rightDir, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(leftDir, OUTPUT);
  pinMode(leftPWM, OUTPUT);
  //pinMode(S0, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(trigger, OUTPUT);
  
  //pinMode(echoLeft, INPUT);
  pinMode(echoRight, INPUT);
  pinMode(echoCenter, INPUT);
  pinMode(OUTRight, INPUT);
  pinMode(OUTLeft, INPUT);
  pinMode(EncR, INPUT_PULLUP); 
  pinMode(EncL, INPUT_PULLUP);

  //digitalWrite(S0,HIGH);//20% frequency scaling
  
  attachInterrupt(digitalPinToInterrupt(EncR), updateEncoderR, RISING);
  attachInterrupt(digitalPinToInterrupt(EncL), updateEncoderL, RISING);
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
  Serial.print("STATE ");
  Serial.println(state);
  uint8_t rawValue = mySensorBar.getRaw();
  Serial.println(rawValue);
  Serial.print("Bin value of input: ");
  for( int i = 6; i >= 1; i-- )
  {
    Serial.print((rawValue >> i) & 0x01);
  }
  Serial.println("b");

  //Print the hex value to the serial buffer.  
  pos = mySensorBar.getPosition();
  Serial.println(pos);
  Serial.print("Density, bits detected (of 8): ");
  density = mySensorBar.getDensity();
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
  if( state==0 ){
    defaultLineFollower();
  }
  else if( state==1 ){
    lookAroundLineFollower();
  }
  else if( state==2 ){
     moveFwd();
     delay(500);
     state = 1;
  }
 
  //stopMoving();
  //delay(666);

}

void lookAroundLineFollower(){
  delay(100);
  lRSpeed = 15;
  Serial.println("TURNING LEFT TO CHECK");
  for( int i = 8; i >= 0; i-- )
  {
    stopMoving();
    turn(-10);
    density = mySensorBar.getDensity();
    delay(100);
    Serial.println(density);
    if( density > 1  ){
      pos = mySensorBar.getPosition();
      Serial.println(pos);
      exit(1);
      stopMoving();
      state = 0;
      pos = mySensorBar.getPosition();
      Serial.println(pos);
      defaultLineFollower();
      return;
    }
  }
  turnBack(80);
  
  //turn right
  Serial.println("TURNING RIGHT TO CHECK");
  for( int i = 8; i >= 0; i-- )
  {
    stopMoving();
    turn(10);
    density = mySensorBar.getDensity();
    delay(100);
    Serial.println(density);
    if( density > 1  ){
      stopMoving();
      pos = mySensorBar.getPosition();
      Serial.println(pos);
      exit(1);
      stopMoving();
      state = 0;
      pos = mySensorBar.getPosition();
      Serial.println(pos);
      defaultLineFollower();
      return;
    }
  }
  turnBack(-80);
  stopMoving();
  state = 2;  
}

void defaultLineFollower(){
  int range = 35;
  if(density == 0){
    stopMoving();
    state = 1;
  }
  else if(pos < range && pos > -range){//go forward
    Serial.println("FORWARD ");
    moveFwd();
    //delay(50);
   }
  else if(pos >= range){//go right
    Serial.print("RIGHT ");
    float strengthRatio = pos / 126.0;
    lRSpeed = 15 + strengthRatio * 60;
    stopMoving();
    //moveRight();
    turn(strengthRatio*40 + 10);
   }
  else if(pos <= -range){//go left
    Serial.print("LEFT ");
    Serial.print(lRSpeed);
    Serial.print(" pos ");
    Serial.print(pos);
    Serial.print(" ratio ");
    double strengthRatio = pos / -126.0;
    Serial.print(strengthRatio);
    //lRSpeed = 15 + strengthRatio * 60;
    stopMoving();
    turn(-strengthRatio*40 - 10);
    
   }
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

float getRightSpeed(){
  rpm = (float)(encoderValueR * 60 / ENC_COUNT_REV);
  encoderValueR = 0;
  //Serial.print("SPEED ");
  //Serial.println(rpm);
  delay(100);
  return rpm;
}

float getLeftSpeed(){
  rpm = (float)(encoderValueL * 60 / ENC_COUNT_REV);
  encoderValueL = 0;
  //Serial.print("SPEED ");
  //Serial.println(rpm);
  delay(100);
  return rpm;
}

void updateEncoderR()
{
  // Increment value for each pulse from encoder
  encoderValueR++;
}

void updateEncoderL()
{
  // Increment value for each pulse from encoder
  encoderValueL++;
}

void stopRight(){
  digitalWrite(rightDir, rightFWr);
  analogWrite(rightPWM, 0);
}

void stopLeft(){
  digitalWrite(leftDir, leftFwr);
  analogWrite(leftPWM, 0);
}

void turn(float deg){
  //220 to -90
  //440 to -180
  deg = map(deg, -180, 180, 440, -440);
  Serial.println("DEG");
  Serial.println(deg);
  float distance = 0.0;
  if(deg > 0.0){
    while(distance < deg){
      digitalWrite(rightDir, rightFWr);
      analogWrite(rightPWM, mSpeed);
      distance = distance + getRightSpeed();
      //Serial.print("SPEED ");
      Serial.println(distance);
    }
    stopRight();
  }
  else if(deg < 0.0){
    deg = -deg;
    while(distance < deg){
      digitalWrite(leftDir, leftFwr);
      analogWrite(leftPWM, mSpeed);
      distance = distance + getLeftSpeed();
      //Serial.print("SPEED ");
      Serial.println(distance);
    }
    stopLeft();
  }
  
}

void turnBack(float deg){
  //220 to -90
  //440 to -180
  if(deg > 0){
    deg = map(deg, 0, 180, 0, 220);
  }
  else{
    deg = map(deg, -180, 0, -800, 0);
  }
  
  Serial.println("DEG");
  Serial.println(deg);
  float distance = 0.0;
  if(deg > 0.0){
    while(distance < deg){
      digitalWrite(rightDir, !rightFWr);
      analogWrite(rightPWM, mSpeed);
      distance = distance + getRightSpeed();
      //Serial.print("SPEED ");
      Serial.println(distance);
    }
    stopRight();
  }
  else if(deg < 0.0){
    deg = -deg;
    while(distance < deg){
      digitalWrite(leftDir, !leftFwr);
      analogWrite(leftPWM, mSpeed);
      distance = distance + getLeftSpeed();
      //Serial.print("SPEED ");
      Serial.println(distance);
    }
    stopLeft();
  }
  
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
