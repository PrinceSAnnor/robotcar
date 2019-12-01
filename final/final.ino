#include "Wire.h"
#include "sensorbar.h"

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

volatile long encoderValueR = 0;
volatile long encoderValueL = 0;

const int TOL = 15;
int toleration = TOL;

const int GREEN = 99000;
const int BLUE = 99000;
const int RED = 99000;
uint8_t leftSensor = 0;
uint8_t rightSensor = 0;
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
  
  attachInterrupt(0, updateEncoderR, RISING);
  attachInterrupt(1, updateEncoderL, RISING);
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
  setForGreen();
  greenRight = pulseIn(OUTRight, LOW);
  greenLeft = pulseIn(OUTLeft, LOW);
  //Serial.print("GRight:");
  //Serial.print(greenRight);
  //Serial.print(" GLeft:");
  //Serial.println(greenLeft);
  delay(100);
  setForBlue();
  blueRight = pulseIn(OUTRight, LOW);
  blueLeft = pulseIn(OUTLeft, LOW);
  //Serial.print("BRight:");
  //Serial.print(blueRight);
  //Serial.print(" BLeft:");
  //Serial.println(blueLeft);
  delay(100);
  setForRed();
  redRight = pulseIn(OUTRight, LOW);
  redLeft = pulseIn(OUTLeft, LOW);
  //Serial.print("RRight:");
  //Serial.print(redRight);
  //Serial.print(" RLeft:");
  //Serial.println(redLeft);
  //delay(100);
  if(greenRight > GREEN && redRight > RED && blueRight > BLUE){
    Serial.println("GREEN ON RIGHT");
    rightSensor = 1;
    state = 1;
  }
  if(greenLeft > GREEN && redLeft > RED && blueLeft > BLUE){
    Serial.println("GREEN ON LEFT");
    leftSensor = 1;
    state = 1;
  }
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
    defaultLineFollower2();
  }
  else if( state==1 ){
    Serial.println("COLOR LOGIC");
    if( leftSensor == 1 && righttSensor = 1 ){//turn 180
      turn(90);
      turnBack(90);
     }
     else if( leftSensor == 1 && righttSensor = 0 ){//turn -90
      turn(-45);
      turnBack(-45);
     }
     else if( leftSensor == 0 && righttSensor = 1 ){//turn 90
      turn(45);
      turnBack(45);
     }
     leftSensor = 0;
     rightSensor = 0;
     state=0;
     //lookAroundLineFollower();
  }
  else if( state==2 ){
     moveFwd();
     delay(300);
  }
 
  //stopMoving();
  //delay(666);

}

void lookAroundLineFollower(){
  delay(100);
  Serial.println("TURNING RIGHT TO CHECK");
  for( int i = 8; i >= 0; i-- )
  {
    stopMoving();
    turnBack(10);
    density = mySensorBar.getDensity();
    delay(100);
    Serial.println(density);
    if( density > 1  ){
      stopMoving();
      Serial.println("LINE ON RIGHT");
      //moveBck();
      //delay(100);
      mSpeed = 40;
      turn(10);
      turnBack(10);
      turn(10);
      turnBack(10);
      turn(10);
      turnBack(10);
      turn(10);
      turnBack(10);
      turn(10);
      turnBack(10);
      turn(10);
      mSpeed = 20;
      //turn(20);
      //turnBack(20);
      moveFwd();
      delay(500);
      stopMoving();
      state = 0;
      //while(1);
      return;
    }
  }
  turn(-80);
  
  //turn right
  Serial.println("TURNING LEFT TO CHECK");
  for( int i = 8; i >= 0; i-- )
  {
    stopMoving();
    turnBack(-10);
    density = mySensorBar.getDensity();
    delay(100);
    Serial.println(density);
    if( density > 1  ){
      stopMoving();
      mSpeed = 40;
      turn(-10);
      turnBack(-10);
      turn(-10);
      turnBack(-10);
      turn(-10);
      turnBack(-10);
      turn(-10);
      turnBack(-10);
      turn(-10);
      turnBack(-10);
      turn(-10);
      mSpeed = 20;
      //turn(20);
      //turnBack(20);
      moveFwd();
      delay(500);
      stopMoving();
      state = 0;
      return;
    }
  }
  turn(80);
  stopMoving();
  state = 2;
  return;  
}

void defaultLineFollower(){
  int range = 35;
  if(density == 0){
    toleration -= 1;
    moveFwd();
    if(toleration < 0){
      stopMoving();
      state = 1;
      toleration = TOL;
     }
  }
  else if(pos < range && pos > -range){//go forward
    Serial.println("FORWARD ");
    toleration = TOL;
    moveFwd();
    delay(100);
   }
  else if(pos >= range){//go left
    Serial.print("RIGHT ");
    toleration = TOL;
    float strengthRatio = pos / 126.0;
    lRSpeed = 15 + strengthRatio * 20;
    stopMoving();
    moveRight();
   }
  else if(pos <= -range){//go right
    double strengthRatio = pos / -126.0;
    toleration = TOL;
    lRSpeed = 15 + strengthRatio * 20;
    stopMoving();
    moveLeft();
   }
   stopMoving();
   return;
}

void defaultLineFollower2(){
  if(density == 0){
    stopMoving();
  }
  else if( getBar(3)==1 || getBar(4)==1 ){//go forward
    Serial.println("FORWARD ");
    moveFwd();
    delay(20);
   }
  else if( getBar(1)==1 || getBar(2)==1 ){//go left
    Serial.print("LEFT ");
    lRSpeed = 15;
    stopMoving();
    moveLeft();
   }
  else if( getBar(5)==1 || getBar(6)==1 ){//go right
    Serial.print("RIGHT ");
    lRSpeed = 15;
    stopMoving();
    moveRight();
   }
   stopMoving();
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

void turn(int deg){
  if(deg > 0){
    deg = map(deg, 0, 180, 0, -3100);
  }
  else{
    deg = map(deg, -180, 0, 3200, 0);
  }
  if(deg > 0){
    encoderValueR = 0;
    digitalWrite(rightDir, rightFWr);
    analogWrite(rightPWM, mSpeed);
    while(encoderValueR < deg);
    stopRight();
  }
  else if(deg < 0){
    deg = -deg;
    encoderValueL = 0;
    digitalWrite(leftDir, leftFwr);
    analogWrite(leftPWM, mSpeed);
    while(encoderValueL < deg);
    stopLeft();
  }
  
}

void turnBack(int deg){
  if(deg > 0){
    deg = map(deg, 0, 180, 0, -1600);
  }
  else{
    deg = map(deg, -180, 0, 8500, 0);
  }
  if(deg < 0){
    deg = -deg;
    encoderValueR = 0;
    digitalWrite(rightDir, !rightFWr);
    analogWrite(rightPWM, mSpeed);
    while(encoderValueR < deg);
    stopRight();
  }
  else if(deg > 0){
    encoderValueL = 0;
    digitalWrite(leftDir, !leftFwr);
    analogWrite(leftPWM, mSpeed);
    while(encoderValueL < deg);
    stopLeft();
  }
  
}

void moveLeft()
{
  digitalWrite(rightDir, rightFWr);
  analogWrite(rightPWM, lRSpeed);
  digitalWrite(leftDir, !leftFwr);
  analogWrite(leftPWM, lRSpeed);
  delay(200);
}

void moveRight()
{
  digitalWrite(rightDir, !rightFWr);
  analogWrite(rightPWM, lRSpeed);
  digitalWrite(leftDir, leftFwr);
  analogWrite(leftPWM, lRSpeed);
  delay(500);
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
int getBar(int i){
 return (rawValue >> i) & 0x01;  
}
