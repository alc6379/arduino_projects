const int RED = 13;
const int YELLOW = 12;
const int GREEN = 11;
const int SOUND_SENSOR = 2 ;

int state = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(SOUND_SENSOR, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int soundLevel = digitalRead(SOUND_SENSOR);
  //  if (soundLevel > 0 )
  //  {
  //    digitalWrite(GREEN, HIGH);
  //  }
  //  if (soundLevel > 100 )
  //  {
  //    digitalWrite(YELLOW, HIGH);
  //  }
  //  if (soundLevel > 200) {
  //    digitalWrite(RED, HIGH);
  //  }
  //  delay(100);
  if (soundLevel == 1)
  {
    if (state == 0)
    {
      state = 1;
    } else {
      state = 0;
    }
  }

  Serial.println(state, DEC);
  resetAllLeds();
}

void resetAllLeds()
{
  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);
}

