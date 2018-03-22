#define RELAY_PIN 2
#define IR_PIN 1
#define BUTTON_PIN 0


byte buttonState = HIGH; //this variable tracks the state of the button, low if not pressed, high if pressed
long lastDebounceTime = 0;  // the last time the output pin was toggled
const byte debounceDelay = 50;    // the debounce time; increase if the output flickers

#define IRpin_PIN PINB // ATTiny85 had Port B pins
#define IRpin 2

#define MAXPULSE    5000  // the maximum pulse we'll listen for - 5 milliseconds 
#define NUMPULSES    50  // max IR pulse pairs to sample
#define RESOLUTION     2  // // time between IR measurements

// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[NUMPULSES][2]; // pair is high and low pulse
uint16_t currentpulse = 0; // index for pulses we're storing
uint32_t irCode = 0;

byte lightState = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin RELAY_PIN as an output.
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(RELAY_PIN, lightState);
}

// the loop function runs over and over again forever
void loop() {

  // Get the updated value :
  byte value = digitalRead(BUTTON_PIN);

  // Turn on or off the LED as determined by the state :
  if ( (millis() - lastDebounceTime) > debounceDelay) {
    if (value == LOW) {
      lightState = !lightState;
      digitalWrite(RELAY_PIN, lightState );
      lastDebounceTime = millis();
    }
  }
  else {
    uint16_t numpulse = listenForIR(); // Wait for an IR Code

    // Process the pulses to get a single number representing code
    for (int i = 0; i < 32; i++) {
      irCode = irCode << 1;
      if ((pulses[i][0] * RESOLUTION) > 0 && (pulses[i][0] * RESOLUTION) < 500) {
        irCode |= 0;
      } else {
        irCode |= 1;
      }
    }
    if (irCode == 0xB5E9B811)
    {
      lightState = !lightState;
      digitalWrite(RELAY_PIN, lightState);
    }
  }

  //  delay(100);
}

uint16_t listenForIR() {  // IR receive code
  currentpulse = 0;
  while (1) {
    unsigned int highpulse, lowpulse;  // temporary storage timing
    highpulse = lowpulse = 0; // start out with no pulse length

    while (IRpin_PIN & _BV(IRpin)) { // got a high pulse
      highpulse++;
      delayMicroseconds(RESOLUTION);
      if (((highpulse >= MAXPULSE) && (currentpulse != 0)) || currentpulse == NUMPULSES) {
        return currentpulse;
      }
    }
    pulses[currentpulse][0] = highpulse;

    while (! (IRpin_PIN & _BV(IRpin))) { // got a low pulse
      lowpulse++;
      delayMicroseconds(RESOLUTION);
      if (((lowpulse >= MAXPULSE) && (currentpulse != 0)) || currentpulse == NUMPULSES) {
        return currentpulse;
      }
    }
    pulses[currentpulse][1] = lowpulse;
    currentpulse++;
  }
}
