#include <IRremote.h>;
#include <Bounce2.h>;

const int SWITCH_PIN = 2;
const int IR_PIN = 13;
const int BUTTON_PIN = 5;


int lightState = 0;
int remoteTriggered = 0;

IRrecv irrecv(IR_PIN); //Creates a variable of type IRrecv
decode_results results;
// Instantiate a Bounce object
Bounce debouncer = Bounce();

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Serial.println(SWITCH_PIN);
  // initialize digital pin SWITCH_PIN as an output.
  pinMode(SWITCH_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // After setting up the button, setup the Bounce instance :
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(1); // interval in ms

  digitalWrite(SWITCH_PIN, lightState);

  irrecv.enableIRIn();
}

// the loop function runs over and over again forever
void loop() {

  // Update the Bounce instance :
  debouncer.update();

  // Get the updated value :
  int value = debouncer.read();

  // Turn on or off the LED as determined by the state :
  if ( value == LOW ) {
    lightState = !lightState;
    digitalWrite(SWITCH_PIN, lightState );
  }
  else {
    if (irrecv.decode(&results)) //if the ir receiver module receiver data

    {
      Serial.print("irCode: "); //print"irCode: "
      Serial.print(results.value, HEX); //print the value in hexdecimal
      Serial.print(", bits: "); //print" , bits: "
      Serial.println(results.bits); //print the bits
      irrecv.resume(); // Receive the next value
      remoteTriggered = 1;
      //delay 600ms
    }


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
  }

  //  delay(100);
}
