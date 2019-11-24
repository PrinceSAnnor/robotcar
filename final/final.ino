int lRSpeed = 90;
int mSpeed = 20;
int leftPWM = 6;
int leftDir = 7;
int rightPWM = 9;
int rightDir = 8;
int leftFwr = LOW;
int rightFWr = HIGH;

void moveLeft()
{
  digitalWrite(rightDir, rightFWr);
  analogWrite(rightPWM, lRSpeed);
  digitalWrite(leftDir, !leftFwr);
  analogWrite(leftPWM, lRSpeed);
  delay(500);
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
  delay(500);
}

void stopMoving()
{
  digitalWrite(rightDir, rightFWr);
  analogWrite(rightPWM, 0);
  digitalWrite(leftDir, leftFwr);
  analogWrite(leftPWM, 0);
}

void setup()
{
  pinMode(rightDir, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(leftDir, OUTPUT);
  pinMode(leftPWM, OUTPUT);
  moveFwd();
  delay(3000);
  moveLeft();
  moveFwd();
  delay(3000);
  moveRight();
  moveBck();
  delay(3000);
  stopMoving();

}

void loop()
{
}
