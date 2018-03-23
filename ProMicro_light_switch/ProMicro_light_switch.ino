#include <IRremote.h>;
#include <Bounce2.h>;

const int RELAY_PIN = A0;
const int IR_PIN = 10;
const int BUTTON_PIN = 4;


int lightState = 0;
int remoteTriggered = 0;

IRrecv irrecv(IR_PIN); //Creates a variable of type IRrecv
decode_results results;
// Instantiate a Bounce object
Bounce debouncer = Bounce();

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Serial.println(RELAY_PIN);
  // initialize digital pin RELAY_PIN as an output.
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // After setting up the button, setup the Bounce instance :
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(100); // interval in ms

  digitalWrite(RELAY_PIN, lightState);
  digitalWrite(LED_BUILTIN, lightState );

  irrecv.enableIRIn();
}

// the loop function runs over and over again forever
void loop() {

  // Update the Bounce instance :
  debouncer.update();

  // Turn on or off the LED as determined by the state :
  if ( debouncer.fell() ) {
    lightState = !lightState;
    Serial.println("Button triggered light");
    digitalWrite(RELAY_PIN, lightState );
    digitalWrite(LED_BUILTIN, lightState );
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
        digitalWrite(RELAY_PIN, lightState);
        digitalWrite(LED_BUILTIN, lightState );
        remoteTriggered = 0;
      }
    }
  }

  //  delay(100);
}
