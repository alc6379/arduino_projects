#include <Adafruit_NeoPixel.h>
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
#include <avr/power.h>
#endif

#define PIN            4
#define NUMPIXELS      12
#define BUTTON         0
#define BATTERY_PIN    2
#define DELAY_VALUE    250 // delay between making low battery lights toggle
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN);

int currentColor = 0;
int currentPixel = 0;

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

const int errorLights[] = {1, 4, 7, 10};


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
  clearPixels(); 
  pixels.setPixelColor(0, 128, 0, 0);
  pixels.show();
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BATTERY_PIN, INPUT);

  //  notifyLowBattery();

}

void loop() {
  if (digitalRead(BATTERY_PIN) == 0) {
    notifyLowBattery();
  }
  else
  {
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
}

void clearPixels() {
  for (int i = 0; i < 12; i++) //12 is the number of LEDs
  {

    pixels.setPixelColor(i, 0x000000);
  }

  pixels.show();
  delay(30);
}

void notifyLowBattery() {

  for (int j = 0; j < 3; j++)
  {

    clearPixels();
    for (int i = j;  i < 12; i = i + 3) {
      pixels.setPixelColor(i, 128, 0, 0);

    }

    pixels.show();

    delay(DELAY_VALUE);

  }

}

