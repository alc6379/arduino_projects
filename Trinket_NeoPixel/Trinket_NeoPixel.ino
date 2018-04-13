#include <Adafruit_NeoPixel.h>
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
#include <avr/power.h>
#endif

#define PIN            2
#define NUMPIXELS      12
#define BUTTON         3
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN);

int currentColor = 0;
int currentPixel = -1;

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  pixels.setBrightness(20); // 1/3 brightness
  pixels.show();
  pinMode(BUTTON, INPUT_PULLUP);

}

void loop() {

  int reading = digitalRead(BUTTON);

  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        currentPixel++;

        if (currentPixel >= NUMPIXELS) {
          currentPixel = 0;

          currentColor++;

          if (currentColor > 2) {
            currentColor = 0;
          }

        }

        switch (currentColor) {
          case 0:
            pixels.setPixelColor(currentPixel, 128, 0, 0);
            break;
          case 1:
            pixels.setPixelColor(currentPixel, 128, 0, 128);
            break;
          case 2:
            pixels.setPixelColor(currentPixel, 0, 0, 128);
            break;
        }
        pixels.show();
        delay(30);
      }
    }
  }

  lastButtonState = reading;
}
