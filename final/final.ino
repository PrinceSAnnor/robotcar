
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

uint8_t state = 0;

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

int RED = 30;
int GREEN = 22;
int BLUE = 22;
uint8_t leftSensor = 0;
uint8_t rightSensor = 0;

int obstacle = 0;
const long interval = 1000;
unsigned long previousMillis = 0;

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

  digitalWrite(S0, HIGH); //20% frequency scaling

  attachInterrupt(0, updateEncoderR, RISING);
  attachInterrupt(1, updateEncoderL, RISING);
  mySensorBar.setBarStrobe();//only turn on IR during reads
  mySensorBar.clearInvertBits();//Default dark on light
  uint8_t returnStatus = mySensorBar.begin();
  Serial.begin(4800);
  if (returnStatus)
  {
    //Serial.println("sx1509 ok");
  }
  else
  {
    //Serial.println("sx1509 not ok!");
    while (1);
  }

}

void loop()
{

  if (Serial.available() > 0) {
    char temp = Serial.read();
    obstacle = (int) temp;
    if (obstacle == 70)
      state = 2;
  }

  rawValue = mySensorBar.getRaw();
  //Serial.println(rawValue);
  //Serial.print("Bin value of input: ");
  for ( int i = 7; i >= 0; i-- )
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

  if ( state == 0 ) {
    defaultLineFollower();
    delay(300);
  }
  else if ( state == 1 ) {
    int greensR[30];
    int bluesR[30];
    int redsR[30];
    int greensL[30];
    int bluesL[30];
    int redsL[30];
    for ( int i = 0; i < 30; i++ ) {
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
    //Serial.println(" ");
    redRight = getMod(redsR, 30);
    redLeft = getMod(redsL, 30);
    blueRight = getMod(bluesR, 30);
    blueLeft = getMod(bluesL, 30);
    greenRight = getMod(greensR, 30);
    greenLeft = getMod(greensL, 30);
    //Serial.println("left rgb");
    //Serial.println(redLeft);
    //Serial.println(greenLeft);
    //Serial.println(blueLeft);
    //Serial.println("right rgb");
    //Serial.println(redRight);
    //Serial.println(greenRight);
    //Serial.println(blueRight);
    //Serial.println(" ");
    if (greenRight > GREEN && redRight > RED && blueRight > BLUE) {
      //Serial.println("green on right");
      rightSensor = 1;
    }
    if (greenLeft > GREEN && redLeft > RED && blueLeft > BLUE) {
      //Serial.println("green on left");
      leftSensor = 1;
    }
    //Serial.println("color logic");
    if ( leftSensor == 1 && rightSensor == 1 ) { //turn 180
      //Serial.println("180 turn");
      turnTank(180);
      state = 0;
    }
    else if ( leftSensor == 1 && rightSensor == 0 ) { //turn -90
      //Serial.println("-90 turn");
      turnTank(-90);
      state = 0;
    }
    else if ( leftSensor == 0 && rightSensor == 1 ) { //turn 90
      //Serial.println("90 turn");
      turnTank(90);
      state = 0;
    }
    else {
      //Serial.println("continue straight");
      moveFwd();
      delay(100);
      stopMoving();
      state = 0;
    }
    leftSensor = 0;
    rightSensor = 0;
  }
  else if ( state == 2 ) {
    //distance logic
    int orb;
    int orb_counter = 0;
    int counter_disp = 0;

    //distanceForward(20);

    char t = 'a';
    delay(1300);

    t = (int) serialFindFlush('R', 'L');
    orb = t;

    distanceBackward(170);

    if (orb == 76)
    {
      turnTank(90);
      delay(1300);
      t = (int) serialFindFlush11('L');
      orb = t;
      //    Serial.print(orb);
      //    Serial.print(t);

      //(obstacle == 70 | obstacle == 76 || obstacle == 66 || obstacle == 82)

      if (orb != 76)
      {
        distanceForward(50);
      }

      orb_counter = 0;

      while (orb == 76)
      {
        orb_counter++;
        distanceForward(50);
        counter_disp++;
        t = (int) serialFindFlush123('L');
        orb = t;
        delay(1300);
      }

      if (orb_counter == 0)
      {
        distanceForward(200);
      }
      else if (orb_counter == 1)
      {
        distanceForward(200);

      }

      orb_counter = 0;

      turn(-90);

      delay(1300);
      t = 'a';
      t = (int) serialFindFlush123('L');
      orb = t;

      if (orb != 76)
      {
        distanceForward(400);
      }

      orb_counter = 0;
      while (orb == 76)
      {
        orb_counter++;
        distanceForward(50);
        t = (int) serialFindFlush123('L');
        orb = t;
        delay(1500);
      }

      if (orb_counter == 0)
      {
        distanceForward(600);
      }
      else if (orb_counter == 1)
      {
        distanceForward(400);
      }
      else if (orb_counter == 2)
      {
        distanceForward(150);
      }

      delay(1300);
      t = 'a';
      t = (int) serialFindFlush123('L');
      orb = t;

      if (orb_counter <= 2 && orb == 76)
      {
        orb_counter = 3;

        while (orb == 76)
        {
          orb_counter++;
          distanceForward(50);
          t = (int) serialFindFlush123('L');
          orb = t;
          delay(1500);
        }
      }

      if (orb_counter == 4)
      {
        distanceForward(400);
      }

      orb_counter = 0;
      distanceForward(60);
      turn(-90);

      if (counter_disp == 0)
        counter_disp = 2;

      distanceForward(60 * counter_disp);

      turnTank(130);
    }

    else if (orb == 82)
    {
      turnTank(-90);
      delay(1300);
      t = (int) serialFindFlush11('R');
      orb = t;
      //    Serial.print(orb);
      //    Serial.print(t);

      //(obstacle == 70 | obstacle == 76 || obstacle == 66 || obstacle == 82)

      if (orb != 82)
      {
        distanceForward(200);
      }

      orb_counter = 0;

      while (orb == 82)
      {
        orb_counter++;
        distanceForward(50);
        counter_disp++;
        t = (int) serialFindFlush123('R');
        orb = t;
        delay(1300);
      }

      if (orb_counter == 0)
      {
        distanceForward(600);
        delay(100);
        distanceForward(600);
      }
      else if (orb_counter == 1)
      {
        distanceForward(250);
        delay(100);
        distanceForward(250);

      }
      else if (orb_counter == 2)
      {
        distanceForward(150);
      }

      orb_counter = 0;

      turn(90);

      delay(1300);
      t = 'a';
      t = (int) serialFindFlush123('R');
      orb = t;

      if (orb != 82)
      {
        distanceForward(400);
      }

      orb_counter = 0;
      while (orb == 82)
      {
        orb_counter++;
        distanceForward(70);
        t = (int) serialFindFlush123('R');
        orb = t;
        delay(1500);
      }

      if (orb_counter == 0)
      {
        distanceForward(600);
      }
      else if (orb_counter == 1)
      {
        distanceForward(250);
      }
      else if (orb_counter == 2)
      {
        distanceForward(150);
      }

      delay(1300);
      t = 'a';
      t = (int) serialFindFlush123('R');
      orb = t;

      if (orb_counter <= 2 && orb == 82)
      {
        orb_counter = 3;

        while (orb == 82)
        {
          orb_counter++;
          distanceForward(70);
          t = (int) serialFindFlush123('R');
          orb = t;
          delay(1500);
        }
      }

      if (orb_counter == 4)
      {
        distanceForward(400);
      }

      orb_counter = 0;
      distanceForward(400);
      turn(90);

      if (counter_disp == 0)
        counter_disp = 2;

      distanceForward(60 * counter_disp);

      turnTank(-45);
    }

    else
    {
      turnTank(90);
      delay(1300);
      t = (int) serialFindFlush11('L');
      orb = t;
      //    Serial.print(orb);
      //    Serial.print(t);

      //(obstacle == 70 | obstacle == 76 || obstacle == 66 || obstacle == 82)

      if (orb != 76)
      {
        distanceForward(50);
      }

      orb_counter = 0;

      while (orb == 76)
      {
        orb_counter++;
        distanceForward(50);
        counter_disp++;
        t = (int) serialFindFlush123('L');
        orb = t;
        delay(1300);
      }

      if (orb_counter == 0)
      {
        distanceForward(200);
      }
      else if (orb_counter == 1)
      {
        distanceForward(200);

      }

      orb_counter = 0;

      turn(-90);

      delay(1300);
      t = 'a';
      t = (int) serialFindFlush123('L');
      orb = t;

      if (orb != 76)
      {
        distanceForward(400);
      }

      orb_counter = 0;
      while (orb == 76)
      {
        orb_counter++;
        distanceForward(50);
        t = (int) serialFindFlush123('L');
        orb = t;
        delay(1500);
      }

      if (orb_counter == 0)
      {
        distanceForward(600);
      }
      else if (orb_counter == 1)
      {
        distanceForward(400);
      }
      else if (orb_counter == 2)
      {
        distanceForward(150);
      }

      delay(1300);
      t = 'a';
      t = (int) serialFindFlush123('L');
      orb = t;

      if (orb_counter <= 2 && orb == 76)
      {
        orb_counter = 3;

        while (orb == 76)
        {
          orb_counter++;
          distanceForward(50);
          t = (int) serialFindFlush123('L');
          orb = t;
          delay(1500);
        }
      }

      if (orb_counter == 4)
      {
        distanceForward(400);
      }

      orb_counter = 0;
      distanceForward(60);
      turn(-90);

      if (counter_disp == 0)
        counter_disp = 2;

      distanceForward(60 * counter_disp);

      turnTank(130);
    }


    state = 0;
  }

}

char serialFlush() {
  char t;
  while (Serial.available() > 0) {
    t  = Serial.read();
  }
  return t;
}

char serialFindFlush123(char val1) {
  char t;
  int counter = 0;

  while (Serial.available() > 0) {
    t  = Serial.read();
    if (t == val1)
    {
      break;
    }
  }

  while (Serial.available() > 0) {
    char ttemp  = Serial.read();
    counter++;
    if (counter > 100)
    {
      break;
    }
  }
  return t;
}

char serialFindFlush(char val1, char val2) {
  char t;
  while (Serial.available() > 0) {
    t  = Serial.read();
    if (t == val1 || t == val2 )
    {
      break;
    }
  }

  return t;
}

char serialFindFlush11(char val1) {
  char t;
  while (Serial.available() > 0) {
    t  = Serial.read();
    if (t == val1)
    {
      break;
    }
  }

  return t;
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


void stopRight() {
  digitalWrite(rightDir, rightFWr);
  analogWrite(rightPWM, 0);
}

void stopLeft() {
  digitalWrite(leftDir, leftFwr);
  analogWrite(leftPWM, 0);
}

void turn(int deg) {
  mSpeed = 60;
  if (deg > 0) {
    deg = map(deg, 0, 180, 0, -1600);
  }
  else {
    deg = map(deg, -180, 0, 1600, 0);
  }
  if (deg > 0) {
    encoderValueR = 0;
    digitalWrite(rightDir, rightFWr);
    analogWrite(rightPWM, mSpeed);
    while (encoderValueR < deg);
    stopRight();
  }
  else if (deg < 0) {
    deg = -deg;
    encoderValueL = 0;
    digitalWrite(leftDir, leftFwr);
    analogWrite(leftPWM, mSpeed);
    while (encoderValueL < deg);
    stopLeft();
  }
  mSpeed = 20;
}

void turnBack(int deg) {
  mSpeed = 60;
  if (deg > 0) {
    deg = map(deg, 0, 180, 0, -1600);
  }
  else {
    deg = map(deg, -180, 0, 1600, 0);
  }
  if (deg < 0) {
    deg = -deg;
    encoderValueR = 0;
    digitalWrite(rightDir, !rightFWr);
    analogWrite(rightPWM, mSpeed);
    while (encoderValueR < deg);
    stopRight();
  }
  else if (deg > 0) {
    encoderValueL = 0;
    digitalWrite(leftDir, !leftFwr);
    analogWrite(leftPWM, mSpeed);
    while (encoderValueL < deg);
    stopLeft();
  }
  mSpeed = 20;
}

void turnTank(int deg) {
  mSpeed = 60;
  if (deg > 0) {
    deg = map(deg, 0, 180, 0, -750);
  }
  else {
    deg = map(deg, -180, 0, 750, 0);
  }
  if (deg < 0) {
    deg = -deg;
    encoderValueR = 0;
    encoderValueL = 0;
    digitalWrite(rightDir, !rightFWr);
    analogWrite(rightPWM, mSpeed);
    digitalWrite(leftDir, leftFwr);
    analogWrite(leftPWM, mSpeed);
    while (encoderValueR < deg  || encoderValueL < deg);
    stopRight();
    stopLeft();
  }
  else if (deg > 0) {
    encoderValueR = 0;
    encoderValueL = 0;
    digitalWrite(rightDir, rightFWr);
    analogWrite(rightPWM, mSpeed);
    digitalWrite(leftDir, !leftFwr);
    analogWrite(leftPWM, mSpeed);
    while (encoderValueL < deg || encoderValueR < deg);
    stopLeft();
    stopRight();
  }
  mSpeed = 20;
}

void distanceForward(int distance) {
  encoderValueR = 0;
  encoderValueL = 0;
  digitalWrite(rightDir, rightFWr);
  analogWrite(rightPWM, mSpeed);
  digitalWrite(leftDir, leftFwr);
  analogWrite(leftPWM, mSpeed);
  while (encoderValueL < distance || encoderValueR < distance) {
    if ( encoderValueL < encoderValueR  ) {
      stopRight();
      while ( (encoderValueR - encoderValueL) > 0 );
      digitalWrite(rightDir, rightFWr);
      analogWrite(rightPWM, mSpeed);
    }
    if ( encoderValueL > encoderValueR  ) {
      stopLeft();
      while ( (encoderValueL - encoderValueR) > 0 );
      digitalWrite(leftDir, leftFwr);
      analogWrite(leftPWM, mSpeed);
    }
  }
  stopLeft();
  stopRight();
}

void distanceBackward(int distance) {
  encoderValueR = 0;
  encoderValueL = 0;
  digitalWrite(rightDir, !rightFWr);
  analogWrite(rightPWM, mSpeed);
  digitalWrite(leftDir, !leftFwr);
  analogWrite(leftPWM, mSpeed);
  while (encoderValueL < distance || encoderValueR < distance) {
    if ( encoderValueL < encoderValueR  ) {
      stopRight();
      while ( (encoderValueR - encoderValueL) > 0 );
      digitalWrite(rightDir, !rightFWr);
      analogWrite(rightPWM, mSpeed);
    }
    if ( encoderValueL > encoderValueR  ) {
      stopLeft();
      while ( (encoderValueL - encoderValueR) > 0 );
      digitalWrite(leftDir, !leftFwr);
      analogWrite(leftPWM, mSpeed);
    }
  }
  stopLeft();
  stopRight();
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

void intersectionMove() {
  distanceForward( 120 );
}

void setForGreen() {
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
}
void setForBlue() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
}
void setForRed() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
}
int getBar(int i) {
  return (rawValue >> i) & 0x01;
}

int getMod(int arr[], int siz) {
  int maxNum = -999;
  int maxInd = -1;
  int count = 0;
  for ( int i = 0; i < siz; i++ ) {
    count = 0;
    for ( int k = 0; k < siz; k++ ) {
      if ( arr[k] == arr[i] ) {
        count = count + 1;
      }
    }
    if (count > maxNum) {
      maxNum = count;
      maxInd = i;
    }
  }
  return arr[maxInd];
}

int checkedUp = 0;
void defaultLineFollower() {
  mSpeed = 40;
  if ( getBar(0) == 1 && getBar(1) == 1 && getBar(2) == 1 && getBar(3) == 1 && getBar(4) == 1 && getBar(5) == 1 && getBar(6) == 1 && getBar(7) == 1) { //intersection
    //Serial.println("intersection ");
    intersectionMove();
    state = 1;
    return;
  }
  else if ( getBar(3) == 1 && getBar(4) == 1) { //clear go forward
    if ( getBar(0) == 1 && getBar(1) == 1 && getBar(2) == 1) { //needs a sharp turn to right
      //Serial.println("sharp right");
      if ( checkedUp < 12 ) {
        stopMoving();
        distanceForward(100);
        stopMoving();
        rawValue = mySensorBar.getRaw();
        //Serial.println("right intersection checkup");
        for ( int i = 7; i >= 0; i-- ) {
          //Serial.print((rawValue >> i) & 0x01);
        }
        distanceBackward(100);
        stopMoving();
        checkedUp = 42;
        if ( (getBar(3) == 1 && getBar(4) == 1 && getBar(5) == 0 && getBar(2) == 0) || (getBar(3) == 1 && getBar(4) == 0 && getBar(5) == 0 && getBar(2) == 1) || (getBar(3) == 0 && getBar(4) == 1 && getBar(5) == 1 && getBar(2) == 0) ) {
          //Serial.println("intersection");
          intersectionMove();
          state = 1;
          return;
        }
        rawValue = mySensorBar.getRaw();
      }
      while ( getBar(0) == 1 ) {
        rawValue = mySensorBar.getRaw();
        turnTank(1);
      }
      while (getBar(4) == 0 || getBar(3) == 0 ) {
        rawValue = mySensorBar.getRaw();
        distanceForward(3);
        stopMoving();
      }
      if ( getBar(6) == 1 && getBar(7) == 1  ) {
        while (getBar(4) == 1 || getBar(3) == 1 ) {
          rawValue = mySensorBar.getRaw();
          distanceForward(3);
          stopMoving();
        }
        while (getBar(4) == 0 || getBar(3) == 0 ) {
          rawValue = mySensorBar.getRaw();
          turnTank(1);
        }
      }
    }
    else if ( getBar(5) == 1 && getBar(6) == 1 && getBar(7) == 1 ) { //needs a sharp turn to left
      //Serial.println("sharp left ");
      if ( checkedUp < 12 ) {
        stopMoving();
        distanceForward(100);
        stopMoving();
        rawValue = mySensorBar.getRaw();
        //Serial.println("left intersection checkup");
        for ( int i = 7; i >= 0; i-- ) {
          //Serial.print((rawValue >> i) & 0x01);
        }
        distanceBackward(100);
        stopMoving();
        checkedUp = 42;
        if ( (getBar(3) == 1 && getBar(4) == 1 && getBar(5) == 0 && getBar(2) == 0) || (getBar(3) == 1 && getBar(4) == 0 && getBar(5) == 0 && getBar(2) == 1) || (getBar(3) == 0 && getBar(4) == 1 && getBar(5) == 1 && getBar(2) == 0) ) {
          //Serial.println("intersection");
          intersectionMove();
          state = 1;
          return;
        }
        rawValue = mySensorBar.getRaw();
      }
      while ( getBar(7) == 1 ) {
        rawValue = mySensorBar.getRaw();
        turnTank(-1);
      }
      while (getBar(4) == 0 || getBar(3) == 0 ) {
        rawValue = mySensorBar.getRaw();
        distanceForward(3);
        stopMoving();
      }
      if ( getBar(0) == 1 && getBar(1) == 1  ) {
        while (getBar(4) == 1 || getBar(3) == 1 ) {
          rawValue = mySensorBar.getRaw();
          distanceForward(3);
          stopMoving();
        }
        while (getBar(4) == 0 || getBar(3) == 0 ) {
          rawValue = mySensorBar.getRaw();
          turnTank(-1);
        }
      }
    }
    else {
      //Serial.println("forward ");
      distanceForward(10);
    }
  }
  else if ( getBar(3) == 1 || getBar(4) == 1 ) { //align then go forward
    //Serial.println("align forward");
    if (getBar(3) == 1 ) { //find bar 4
      while ( getBar(4) == 0 ) {
        rawValue = mySensorBar.getRaw();
        turnTank(1);
      }
    }
    else { //find bar three
      while ( getBar(3) == 0 ) {
        rawValue = mySensorBar.getRaw();
        turnTank(-1);
      }
    }
    distanceForward(10);
  }
  else if ( getBar(2) == 1) { //recovery towards left
    distanceForward(10);
    stopMoving();
    while ( getBar(4) == 0 && getBar(3) == 0 ) {
      rawValue = mySensorBar.getRaw();
      turnTank(1);
    }
  }
  else if ( getBar(5) == 1) { //recovery towards right
    distanceForward(10);
    stopMoving();
    while ( getBar(4) == 0 && getBar(3) == 0) {
      rawValue = mySensorBar.getRaw();
      turnTank(-1);
    }
  }
  else if ( getBar(0) == 0 && getBar(1) == 0 && getBar(2) == 0 && getBar(3) == 0 && getBar(4) == 0 && getBar(5) == 0 && getBar(6) == 0 && getBar(7) == 0 ) { //nothing
    //Serial.println("nothing detected should be a gap");
    distanceForward(10);
  }
  mSpeed = 20;
  checkedUp = checkedUp - 2;
  stopMoving();
}
