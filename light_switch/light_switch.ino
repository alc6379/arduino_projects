#include <IRremote.h>;

const int SWITCH_PIN = 2;
const int IR_PIN = 13;


int lightState = 0;
int remoteTriggered = 0;

IRrecv irrecv(IR_PIN); //Creates a variable of type IRrecv
decode_results results;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Serial.println(SWITCH_PIN);
  // initialize digital pin SWITCH_PIN as an output.
  pinMode(SWITCH_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);

  digitalWrite(SWITCH_PIN, lightState);

  irrecv.enableIRIn();
}

// the loop function runs over and over again forever
void loop() {

  if (irrecv.decode(&results)) //if the ir receiver module receiver data
  {
    Serial.print("irCode: "); //print"irCode: "
    Serial.print(results.value, HEX); //print the value in hexdecimal
    Serial.print(", bits: "); //print" , bits: "
    Serial.println(results.bits); //print the bits
    irrecv.resume(); // Receive the next value
    remoteTriggered = 1;
  }
  delay(100); //delay 600ms

  //  if (results.value == 0xFFA25D)
  if (results.value == 0xB5E9B811)
  {
    if (remoteTriggered == 1) {
      lightState = !lightState;
      Serial.print("new light state: ");
      Serial.print(lightState);
      Serial.println();
      digitalWrite(SWITCH_PIN, lightState);
      remoteTriggered = 0;
    }
  }


  //  digitalWrite(SWITCH_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  //  delay(1000);                       // wait for a second
  //  digitalWrite(SWITCH_PIN, LOW);    // turn the LED off by making the voltage LOW
  //  delay(1000);                       // wait for a second
}
