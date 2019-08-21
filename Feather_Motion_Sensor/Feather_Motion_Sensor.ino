#include <SPI.h>
#include <RH_RF69.h>
#include <LowPower.h>

#define RED 15
#define GREEN 14
#define PIR 2
#define VBATPIN       A9

#define MSEC_DELAY 5000

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 434.0

#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
#define RFM69_CS      8
#define RFM69_INT     7
#define RFM69_RST     4
#define LED           13
#endif

#if defined(ARDUINO_SAMD_FEATHER_M0) // Feather M0 w/Radio
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
#define LED           13
#endif

const long batteryThreshold = 3.1;
const int batteryMsgLength = 4;
const char radiopacket[] = "kitchen|battery|";
const char controlMsg[] = "kitchen|motion|1";

volatile long measuredMsec;
volatile int detected = 0;

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

void setup() {
  Serial.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(RED, HIGH);

  measuredMsec = millis();

  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  rf69.setTxPower(14, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                  };
  rf69.setEncryptionKey(key);

  Serial.println("Initialized");

  sendMessage("kitchen|status|initialized");

}

void loop() {

  int interruptNum = digitalPinToInterrupt(PIR);
  

  //turn off the motionDetected
  if (detected == true) {
    delay(1000); 
    digitalWrite(GREEN, LOW);
    detected = false;
    sendMessage(controlMsg);
  }

  delay(50);

  float batteryReading = checkBattery();

  int msgLength = sizeof(radiopacket) + batteryMsgLength;

  char* batteryMessage = batteryToString(batteryReading);
  sendMessage(batteryMessage);




  if (batteryReading < batteryThreshold) {
    digitalWrite(RED, HIGH);
  } else
  {
    digitalWrite(RED, LOW);
  }

  attachInterrupt(interruptNum, motionDetected, RISING);

  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

  detachInterrupt(interruptNum);

}

void redGreen() {
  digitalWrite(RED, HIGH);
  delay(100);
  digitalWrite(RED, LOW);
  delay(100);
  digitalWrite(GREEN, HIGH);
  delay(100);
  digitalWrite(GREEN, LOW);
  delay(100);
}

void motionDetected() {
  measuredMsec = millis();
  detected = true;
  digitalWrite(GREEN, HIGH);
}

float checkBattery()
{
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage

  return measuredvbat;
}

void sendMessage(char* strbuff) {
  Serial.print("Sending "); Serial.println(strbuff);

  // Send a message!
  rf69.send((uint8_t *)strbuff, strlen(strbuff));
  rf69.waitPacketSent();

}

char* batteryToString(float batteryReading ) {

  char batteryValue[batteryMsgLength];
  char msgbuffer[sizeof(radiopacket) + sizeof(batteryValue)];

  strcpy(msgbuffer, radiopacket);

  dtostrf(batteryReading, 4, 2, batteryValue);
  strcat(msgbuffer, batteryValue);

  return msgbuffer;
}

