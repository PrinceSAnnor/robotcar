#include "Wire.h"
#include "sensorbar.h"

#define trigger A0
#define EncR 2 
#define EncL 3 
#define S2 4
#define S3 5
#define leftPWM 6
#define leftDir 7
#define rightDir 8
#define rightPWM 9
#define OUTRight 10
#define OUTLeft 11
#define S0 12

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
uint8_t rawValue = 0;

volatile long encoderValueR = 0;
volatile long encoderValueL = 0;

const int TOL = 15;
int toleration = TOL;

int GREENRight = -3400;
int BLUERight = -10000;
int REDRight = 24000;
int GREENLeft = -3400;
int BLUELeft = -10000;
int REDLeft = 24000;
int colorTol = 2000;
uint8_t leftSensor = 0;
uint8_t rightSensor = 0;
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
  
  //pinMode(echoLeft, INPUT);
  pinMode(OUTRight, INPUT);
  pinMode(OUTLeft, INPUT);
  pinMode(EncR, INPUT_PULLUP); 
  pinMode(EncL, INPUT_PULLUP);

  digitalWrite(S0,HIGH);//20% frequency scaling
  
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
  state = 1;
  /*
  int greensR[30];
    int bluesR[30];
    int redsR[30];
    int greensL[30];
    int bluesL[30];
    int redsL[30];
    for( int i = 0; i < 30; i++ ){
      setForGreen();
      greenRight = pulseIn(OUTRight, LOW);
      greenLeft = pulseIn(OUTLeft, LOW);
      greensR[i] = greenRight;
      greensL[i] = greenLeft;
      delay(100);
      setForBlue();
      blueRight = pulseIn(OUTRight, LOW);
      blueLeft = pulseIn(OUTLeft, LOW);
      bluesR[i] = blueRight;
      bluesL[i] = blueLeft;
      delay(100);
      setForRed();
      redRight = pulseIn(OUTRight, LOW);
      redLeft = pulseIn(OUTLeft, LOW);
      redsR[i] = redRight;
      redsL[i] = redLeft;
      delay(100);
    }
    Serial.println(" ");
    redRight = getMod(redsR,30);
    redLeft = getMod(redsL,30);
    blueRight = getMod(bluesR,30);
    blueLeft = getMod(bluesL,30);
    greenRight = getMod(greensR,30);
    greenLeft = getMod(greensL,30);
    Serial.println("LEFT RGB");
    Serial.println(redLeft);
    Serial.println(greenLeft);
    Serial.println(blueLeft);
    Serial.println("RIGHT RGB");
    Serial.println(redRight);
    Serial.println(greenRight);
    Serial.println(blueRight);
*/
  
  //Serial.print("STATE ");
  //Serial.println(state);
  rawValue = mySensorBar.getRaw();
  //Serial.println(rawValue);
  //Serial.print("Bin value of input: ");
  for( int i = 7; i >= 0; i-- )
  {
    //Serial.print((rawValue >> i) & 0x01);
  }
  //Serial.println("b");

  //Print the hex value to the serial buffer.  
  pos = mySensorBar.getPosition();
  //Serial.println(pos);
  //Serial.print("Density, bits detected (of 8): ");
  density = mySensorBar.getDensity();
  //Serial.println(density);
  //Serial.println("");
  //delay(300);
  
  if( state==0 ){
    //defaultLineFollower();
    //delay(300);
  }
  else if( state==1 ){
    int greensR[30];
    int bluesR[30];
    int redsR[30];
    int greensL[30];
    int bluesL[30];
    int redsL[30];
    for( int i = 0; i < 30; i++ ){
      setForGreen();
      greenRight = pulseIn(OUTRight, LOW);
      greenLeft = pulseIn(OUTLeft, LOW);
      greensR[i] = greenRight;
      greensL[i] = greenLeft;
      delay(100);
      setForBlue();
      blueRight = pulseIn(OUTRight, LOW);
      blueLeft = pulseIn(OUTLeft, LOW);
      bluesR[i] = blueRight;
      bluesL[i] = blueLeft;
      delay(100);
      setForRed();
      redRight = pulseIn(OUTRight, LOW);
      redLeft = pulseIn(OUTLeft, LOW);
      redsR[i] = redRight;
      redsL[i] = redLeft;
      delay(100);
    }
    Serial.println(" ");
    redRight = getMod(redsR,30);
    redLeft = getMod(redsL,30);
    blueRight = getMod(bluesR,30);
    blueLeft = getMod(bluesL,30);
    greenRight = getMod(greensR,30);
    greenLeft = getMod(greensL,30);
    Serial.println("LEFT RGB");
    Serial.println(redLeft);
    Serial.println(greenLeft);
    Serial.println(blueLeft);
    Serial.println("RIGHT RGB");
    Serial.println(redRight);
    Serial.println(greenRight);
    Serial.println(blueRight);
    Serial.println(" ");
    if(greenRight > GREENRight && redRight > REDRight && blueRight > BLUERight && greenRight < GREENRight+colorTol && redRight < REDRight+colorTol && blueRight < BLUERight+colorTol){
      Serial.println("GREEN ON RIGHT");
      rightSensor = 1;
    }
    if(greenLeft > GREENLeft && redLeft > REDLeft && blueLeft > BLUELeft && greenLeft < GREENLeft+colorTol && redLeft < REDLeft+colorTol && blueLeft < BLUELeft+colorTol){
      Serial.println("GREEN ON LEFT");
      leftSensor = 1;
    }
    Serial.println("COLOR LOGIC");
    if( leftSensor == 1 && rightSensor == 1 ){//turn 180
      Serial.println("180 TURN");
      turnTank(180);
      state=0;
     }
     else if( leftSensor == 1 && rightSensor == 0 ){//turn -90
      Serial.println("-90 TURN");
      turnTank(-90);
      state=0;
     }
     else if( leftSensor == 0 && rightSensor == 1 ){//turn 90
      Serial.println("90 TURN");
      turnTank(90);
      state=0;
     }
     else{
      Serial.println("CONTINUE STRAIGHT");
      moveFwd();
      delay(100);
      stopMoving();
      state=0;
     }
     leftSensor = 0;
     rightSensor = 0;
  }
  else if( state==2 ){
     //distance logic
  }
 
  //stopMoving();
  //delay(666);

}

void defaultLineFollower(){
  mSpeed = 40;
  if( getBar(5)==1 ){//go left
    if( getBar(4)==1 && getBar(6)==1 && getBar(7)==1 ){//90 degree turn left
        stopMoving();
        moveFwd();
        delay(300);
        stopMoving();
        delay(200);
        rawValue = mySensorBar.getRaw();
        Serial.println("LEFT INTER CHECKUP");
        for( int i = 7; i >= 0; i-- ){
            Serial.print((rawValue >> i) & 0x01);
        }
        moveBck();
        delay(300);
        stopMoving();
        if( getBar(3)==1 || getBar(4)==1 || getBar(5)==1 || getBar(2)==1 ){
          Serial.print("INTERSECTION");
          intersectionMove();
          state = 1;
          return;
        }
        Serial.println("NO STRAIGHT AFTER CHECKUP");   
        if( !(getBar(1)==1 && getBar(0)==1 && getBar(3)==1 && getBar(2)==1) ){
          stopMoving();
          moveFwd();
          delay(200);
          stopMoving();
          delay(200);
          rawValue = mySensorBar.getRaw();
          for( int i = 7; i >= 0; i-- ){
            Serial.print((rawValue >> i) & 0x01);
          }
       }
      if(getBar(1)==1 && getBar(0)==1 && getBar(3)==1 && getBar(2)==1){
        Serial.print("INTERSECTION");
        intersectionMove();
        state =1;
        return;
      }
      turn(-20);
      turnBack(-20);
      moveFwd();
      delay(200);
      stopMoving();
      //while(1);
    }
    else{
      lRSpeed = 20;
      stopMoving();
      moveLeft();
    }
    Serial.print("LEFT ");
    
   }
   else if(getBar(2)==1 ){//go right   
    if( getBar(1)==1 && getBar(0)==1 && getBar(3)==1 ){//90 degree turn right
        stopMoving();
        moveFwd();
        delay(300);
        stopMoving();
        delay(200);
        rawValue = mySensorBar.getRaw();
        Serial.println("RIGHT INTER CHECKUP");
        for( int i = 7; i >= 0; i-- ){
            Serial.print((rawValue >> i) & 0x01);
        }
        moveBck();
        delay(300);
        stopMoving();
        if( getBar(3)==1 || getBar(4)==1 || getBar(5)==1 || getBar(2)==1){
          Serial.println("INTERSECTION");
          intersectionMove();
          state = 1;
          return;
        }
        Serial.println("NO STRAIGHT AFTER CHECKUP");
        if( !(getBar(4)==1 && getBar(6)==1 && getBar(7)==1 && getBar(5)==1) ){
          stopMoving();
          moveFwd();
          delay(200);
          stopMoving();
          delay(200);
          rawValue = mySensorBar.getRaw();
          for( int i = 7; i >= 0; i-- ){
            Serial.print((rawValue >> i) & 0x01);
          }
       }
      if(getBar(4)==1 && getBar(6)==1 && getBar(7)==1 && getBar(5)==1){
        Serial.print("INTERSECTION");
        intersectionMove();
        state = 1;
        return;
      }
      turn(20);
      turnBack(20);
      moveFwd();
      delay(200);
      stopMoving();
      //while(1);
    }
    else{
      Serial.print("RIGHT ");
      lRSpeed = 20;
      stopMoving();
      moveRight();
    }
   }
  else if(getBar(6)==1 ){//go left
    Serial.print("CRAZY LEFT ");
    while( getBar(5)==0 || getBar(6)==0 || getBar(4)==0 ){
      rawValue = mySensorBar.getRaw();
      for( int i = 7; i >= 0; i-- ){
          Serial.print((rawValue >> i) & 0x01);
        }
      Serial.println(" ");
      turnBack(-1);
      turn(-3);
    }
    turnTank( -30  );
    stopMoving();
    delay(100);
    rawValue = mySensorBar.getRaw();
    Serial.println("CRAZY LEFT FORWARD");
    while( getBar(5)==0 ){
      moveFwd();
      delay(40);
      rawValue = mySensorBar.getRaw();
    }
    stopMoving();
   }
   else if( getBar(1)==1 ){//go right
    Serial.print("CRAZY RIGHT ");
    while( getBar(2)==0 || getBar(1)==0 || getBar(3)==0 ){
      rawValue = mySensorBar.getRaw();
      for( int i = 7; i >= 0; i-- ){
          Serial.print((rawValue >> i) & 0x01);
        }
      Serial.println(" ");
      turnBack(1);
      turn(3);
    }
    turnTank( 30  );
    stopMoving();
    delay(100);
    rawValue = mySensorBar.getRaw();
    Serial.println("CRAZY RIGHT FORWARD");
    while( getBar(2)==0 ){
      moveFwd();
      delay(40);
      rawValue = mySensorBar.getRaw();
    }
    stopMoving();  
   }
   else if( getBar(3)==1 || getBar(4)==1 ){//go forward
    Serial.println("FORWARD ");
    moveFwd();
    delay(40);
   }
   else{//nothing
    Serial.println("NOTHING DETECTED SO FORWARD ");
    moveFwd();
    delay(40);
   }
   mSpeed = 20;
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
  mSpeed = 60;
  if(deg > 0){
    deg = map(deg, 0, 180, 0, -1600);
  }
  else{
    deg = map(deg, -180, 0, 1600, 0);
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
  mSpeed = 20;
}

void turnBack(int deg){
  mSpeed = 60;
  if(deg > 0){
    deg = map(deg, 0, 180, 0, -1600);
  }
  else{
    deg = map(deg, -180, 0, 1600, 0);
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
  mSpeed = 20;
}

void turnTank(int deg){
  mSpeed = 60;
  if(deg > 0){
    deg = map(deg, 0, 180, 0, -750);
  }
  else{
    deg = map(deg, -180, 0, 750, 0);
  }
  if(deg < 0){
    deg = -deg;
    encoderValueR = 0;
    encoderValueL = 0;
    digitalWrite(rightDir, !rightFWr);
    analogWrite(rightPWM, mSpeed);
    digitalWrite(leftDir, leftFwr);
    analogWrite(leftPWM, mSpeed);
    while(encoderValueR < deg  || encoderValueL < deg);
    stopRight();
    stopLeft();
  }
  else if(deg > 0){
    encoderValueR = 0;
    encoderValueL = 0;
    digitalWrite(rightDir, rightFWr);
    analogWrite(rightPWM, mSpeed);
    digitalWrite(leftDir, !leftFwr);
    analogWrite(leftPWM, mSpeed);
    while(encoderValueL < deg || encoderValueR < deg);
    stopLeft();
    stopRight();
  }
  mSpeed = 20;
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

void intersectionMove(){
  stopMoving();
  //while(1);
  moveFwd();
  delay(500);
  stopMoving();
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

int getMod(int arr[],int siz){
  int maxNum= -999;
  int maxInd = -1;
  int count = 0;
  for( int i = 0; i < siz; i++ ){
    count = 0;
      for( int k = 0; k < siz; k++ ){
        if( arr[k] == arr[i] ){
          count = count + 1;
         }
       }
       if(count > maxNum){
        maxNum = count;
        maxInd = i;
       }
  }
  return arr[maxInd];
}
void colorConfigure(){
  
   
}
