#include <NewPing.h>

#define MAX_DISTANCE 400
#define SONAR_ITERATIONS 5

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
const int maxSpeed = 200;
const int minDistance = 40; /* closest distance (in cm) that the vehicle will get to something
                              before trying to turn around */
int reverseDistance = minDistance + 10;

NewPing sonar(trigPin, echoPin, MAX_DISTANCE);

int lastCheckedDistance = minDistance;


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

}



void loop() {
  int distance = getDistance();


  if (distance < minDistance) {
    unsigned long startTime = millis();
    bool executedHardSpin = false;

    brake();

    while (distance < reverseDistance )
    {
      goBackward();
      distance = getDistance();

      unsigned long currentTime = millis();
      if (currentTime > startTime + 5000)
      {
        brake();
        hardLeft();
        distance = getDistance();
        executedHardSpin = true;
      }
    }

    if (executedHardSpin == false)
    {
      brake();
      turnLeft();
    }
  } else
  {
    goForward();
  }
}


void goBackward()
{
  digitalWrite(mtr1_1, HIGH);
  digitalWrite(mtr1_0, LOW);
  digitalWrite(mtr2_1, HIGH);
  digitalWrite(mtr2_0, LOW);
  enableMotors(maxSpeed * .75, maxSpeed * .75);
}

void goForward(int leftSpeed, int rightSpeed)
{
  digitalWrite(mtr1_1, LOW);
  digitalWrite(mtr1_0, HIGH);
  digitalWrite(mtr2_1, LOW);
  digitalWrite(mtr2_0, HIGH);
  enableMotors(leftSpeed, rightSpeed);
}

void hardLeft() {
  digitalWrite(mtr1_1, HIGH);
  digitalWrite(mtr1_0, LOW);
  digitalWrite(mtr2_1, LOW);
  digitalWrite(mtr2_0, HIGH);
  enableMotors(maxSpeed, maxSpeed);
  delay(100);
}

void goForward()
{
  goForward(maxSpeed, maxSpeed);
}

void turnLeft()
{
  Serial.println("turning left");
  //  goForward(minSpeed, maxSpeed);
  goForward(minSpeed, 200);
  delay(1000);
  brake();
  Serial.println("completed left turn");
}

void turnRight()
{
  goForward(maxSpeed, minSpeed);
  delay(100);
  brake();
}

void brake() {
  digitalWrite(mtr1_1, LOW);
  digitalWrite(mtr1_0, LOW);
  digitalWrite(mtr2_1, LOW);
  digitalWrite(mtr2_0, LOW);
  enableMotors(0, 0);
}

void enableMotors(int leftSpeed, int rightSpeed)
{
  analogWrite(mtr1_enable, leftSpeed);
  analogWrite(mtr2_enable, rightSpeed);
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

#define swap(a,b) a ^= b; b ^= a; a ^= b;
#define sort(a,b) if(a>b){ swap(a,b); }

int median(int a, int b, int c, int d, int e)
{
  sort(a, b);
  sort(d, e);
  sort(a, c);
  sort(b, c);
  sort(a, d);
  sort(c, d);
  sort(b, e);
  sort(b, c);
  // this last one is obviously unnecessary for the median
  //sort(d,e);

  return c;
}

