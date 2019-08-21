#include <NewPing.h>
#include <MPU6050_tockn.h>
#include <Wire.h>

#define MAX_DISTANCE 400
#define SONAR_ITERATIONS 5
#define MAX_TILT_ANGLE 20

const float xOffset = -1.68; //TODO: use these instead of auto calibration
const float yOffset = 0.81;
const float zOffset = 0.81;

const int mtr1_0 = 8;
const int mtr1_1 = 9;
const int mtr1_enable = 5;
const int mtr2_0 = 10;
const int mtr2_1 = 11;
const int mtr2_enable = 6;

// defines pins numbers
const int trigPin = 2;
const int echoPin = 3;
const int minSpeed = 100;
const int maxSpeed = 150;
const int minDistance = 15; /* closest distance (in cm) that the vehicle will get to something
                              before trying to turn around */
int reverseDistance = minDistance + 10;

NewPing sonar(trigPin, echoPin, MAX_DISTANCE);
MPU6050 mpu6050(Wire);

void setup() {
  //  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  //  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(mtr1_0, OUTPUT);
  pinMode(mtr1_1, OUTPUT);
  pinMode(mtr1_enable, OUTPUT);
  pinMode(mtr2_0, OUTPUT);
  pinMode(mtr2_1, OUTPUT);
  pinMode(mtr2_enable, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  //  mpu6050.calcGyroOffsets();
  mpu6050.setGyroOffsets(xOffset, yOffset, zOffset);

  goForward(5000); 
  turnLeft();
  turnLeft(); 
  goForward(2000); 
}



void loop() {
//  int distance = getDistance();
//
//  if (distance < minDistance) {
//    unsigned long startTime = millis();
//    brake();
//
//    bool turnEarly = false;
//
//    while (distance < reverseDistance && turnEarly == false)
//    {
//      goBackward(20);
//      distance = getDistance();
//      unsigned long currentTime = millis();
//      if (currentTime > startTime + 5000)
//      {
//        turnEarly = true;
//      }
//    }
//
//    brake();
//    turnLeft();
//    distance = getDistance();
//  } else
//  {
//    goForward(100);
//  }
}


void goBackward( int duration )
{
  mpu6050.update();
  float targetOrientation = mpu6050.getAngleZ();
  int targetTime = millis() + duration;

  while (millis() < targetTime)
  {
    mpu6050.update();

    int currentOrientation = mpu6050.getAngleZ();

    digitalWrite(mtr1_1, HIGH);
    digitalWrite(mtr1_0, LOW);
    digitalWrite(mtr2_1, HIGH);
    digitalWrite(mtr2_0, LOW);


    int leftSpeed = maxSpeed - (currentOrientation - targetOrientation);
    int rightSpeed = maxSpeed + (currentOrientation - targetOrientation);
    enableMotors(leftSpeed, rightSpeed);
  }
}

void goForward(int duration)
{
  mpu6050.update();
  float targetOrientation = mpu6050.getAngleZ();
  int targetTime = millis() + duration;

  while (millis() < targetTime)
  {
    mpu6050.update();

    digitalWrite(mtr1_1, LOW);
    digitalWrite(mtr1_0, HIGH);
    digitalWrite(mtr2_1, LOW);
    digitalWrite(mtr2_0, HIGH);

    int currentOrientation = mpu6050.getAngleZ();

    int leftSpeed = maxSpeed - (currentOrientation - targetOrientation);
    int rightSpeed = maxSpeed + (currentOrientation - targetOrientation);

    enableMotors(leftSpeed, rightSpeed);
  }
  brake(); 
}

void turnLeft() {

  Serial.println("hard left");
  mpu6050.update();

  float origOrientation = mpu6050.getAngleZ();
  float targetOrientation = origOrientation + 60;
  float currentOrientation = origOrientation;

  while (currentOrientation < targetOrientation) {
    digitalWrite(mtr1_1, HIGH);
    digitalWrite(mtr1_0, LOW);
    digitalWrite(mtr2_1, LOW);
    digitalWrite(mtr2_0, HIGH);
    enableMotors(minSpeed, minSpeed);
    delay(5);
    mpu6050.update();
    currentOrientation = mpu6050.getAngleZ();
    Serial.print("Z axis: ");
    Serial.println(currentOrientation);
  }
  brake();
}

void brake() {
  enableMotors(0, 0);
}

void enableMotors(int leftSpeed, int rightSpeed)
{
  bool tilted = isTilted();

  if (!tilted) {
    analogWrite(mtr1_enable, leftSpeed);
    analogWrite(mtr2_enable, rightSpeed);
  }
  else
  {
    brake();
  }
}


int getDistance() {

  long duration;
  float distance;

  duration = sonar.ping_median(SONAR_ITERATIONS);
  distance = (duration / 2) * 0.0343;

  Serial.print("Distance: ");
  Serial.println((int)distance);

  return (int)distance;
}

bool isTilted()
{
  mpu6050.update();

  float xAngle = mpu6050.getAngleX();
  float yAngle = mpu6050.getAngleY();

  int absX = abs(xAngle);

  int absY = abs(yAngle);

  Serial.print("absX: ");
  Serial.print(absX);
  Serial.print("\t");
  Serial.print("absY: ");
  Serial.println(absY);

  return absY > MAX_TILT_ANGLE || absX > MAX_TILT_ANGLE;
}

