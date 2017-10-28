const int RED = 13;
const int YELLOW = 12;
const int GREEN = 11;

void setup() {
  // put your setup code here, to run once:
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(RED, HIGH);
  delay(200);
  digitalWrite(RED, LOW);
  delay(200);
  digitalWrite(YELLOW, HIGH);
  delay(200);
  digitalWrite(YELLOW, LOW);
  delay(200);
  digitalWrite(GREEN, HIGH);
  delay(200);
  digitalWrite(GREEN, LOW);
  delay(200);
}
