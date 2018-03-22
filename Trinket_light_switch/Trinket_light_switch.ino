#include <IRremote.h>;

#define RELAY_PIN 2
#define IR_PIN 1
#define BUTTON_PIN 0


byte buttonState = HIGH; //this variable tracks the state of the button, low if not pressed, high if pressed
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

byte lightState = 0;
byte remoteTriggered = 0;

IRrecv irrecv(IR_PIN); //Creates a variable of type IRrecv
decode_results results;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin RELAY_PIN as an output.
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(RELAY_PIN, lightState);

  irrecv.enableIRIn();
}

// the loop function runs over and over again forever
void loop() {

  // Get the updated value :
  int value = digitalRead(BUTTON_PIN);

  // Turn on or off the LED as determined by the state :
  if ( (millis() - lastDebounceTime) > debounceDelay) {
    if (value == LOW) {
      lightState = !lightState;
      digitalWrite(RELAY_PIN, lightState );
      lastDebounceTime = millis();
    }
  }
  else {
    if (irrecv.decode(&results)) //if the ir receiver module receiver data
    {
      irrecv.resume(); // Receive the next value
      remoteTriggered = 1;
      //delay 600ms
    }


    //  if (results.value == 0xFFA25D)
    if (results.value == 0xB5E9B811)
    {
      if (remoteTriggered == 1) {
        lightState = !lightState;
        digitalWrite(RELAY_PIN, lightState);
        remoteTriggered = 0;
      }
    }
  }

  //  delay(100);
}
