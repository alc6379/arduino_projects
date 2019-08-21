#include "Adafruit_seesaw.h"
Adafruit_seesaw ss;
#define BUTTON_RIGHT 6
#define BUTTON_DOWN  7
#define BUTTON_LEFT  9
#define BUTTON_UP    10
#define BUTTON_SEL   14
uint32_t button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) |
                       (1 << BUTTON_LEFT) | (1 << BUTTON_UP) | (1 << BUTTON_SEL);
#if defined(ESP8266)
#define IRQ_PIN   2
#elif defined(ESP32)
#define IRQ_PIN   14
#elif defined(NRF52)
#define IRQ_PIN   27
#elif defined(TEENSYDUINO)
#define IRQ_PIN   8
#elif defined(ARDUINO_ARCH_WICED)
#define IRQ_PIN   PC5
#else
#define IRQ_PIN   5
#endif
void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!ss.begin(0x49)) {
    Serial.println("ERROR!");
    while (1);
  }
  else {
    Serial.println("seesaw started");
    Serial.print("version: ");
    Serial.println(ss.getVersion(), HEX);
  }
  ss.pinModeBulk(button_mask, INPUT_PULLUP);
  ss.setGPIOInterrupts(button_mask, 1);
  pinMode(IRQ_PIN, INPUT);
}
int last_x = 0, last_y = 0;
void loop() {
  int x = ss.analogRead(2);
  int y = ss.analogRead(3);

  if ( (abs(x - last_x) > 3)  ||  (abs(y - last_y) > 3)) {
    Serial.print(x); Serial.print(", "); Serial.println(y);
    last_x = x;
    last_y = y;


    switch (x)
    {
      case 0 ... 136:
        Serial.println("Forward 4");
        break;
      case 137 ... 265:
        Serial.println("Forward 3");
        break;
      case 266 ... 393:
        Serial.println("Forward 2");
        break;
      case 394 ... 519:
        Serial.println("Forward 0");
        break;
      case 520 ... 523:
        Serial.println("Stop");
        break;
      case 524 ... 648:
        Serial.println("Reverse 1");
        break; 
      case 649 ... 773:
        Serial.println("Reverse 2");
        break; 
      case 774 ... 899: 
        Serial.println("Reverse 3"); 
        break; 
      case 900 ... 1024:
        Serial.println("Reverse 4");
        break; 
      default:
        Serial.println("Invalid input");
        break;
    }
  }

  //  if(!digitalRead(IRQ_PIN)){
  //    uint32_t buttons = ss.digitalReadBulk(button_mask);
  //    //Serial.println(buttons, BIN);
  //    if (! (buttons & (1 << BUTTON_RIGHT))) {
  //      Serial.println("Button A pressed");
  //    }
  //    if (! (buttons & (1 << BUTTON_DOWN))) {
  //      Serial.println("Button B pressed");
  //    }
  //    if (! (buttons & (1 << BUTTON_LEFT))) {
  //      Serial.println("Button Y pressed");
  //    }
  //    if (! (buttons & (1 << BUTTON_UP))) {
  //      Serial.println("Button x pressed");
  //    }
  //    if (! (buttons & (1 << BUTTON_SEL))) {
  //      Serial.println("Button SEL pressed");
  //    }
  //  }



  delay(10);
}
