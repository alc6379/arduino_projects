#include <SPI.h>
#include <RH_RF69.h>
#include <Ethernet.h>
#include <PubSubClient.h>

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 434.0

#define RFM69_INT     2  //
#define RFM69_CS      9  //
#define RFM69_RST     7  // "A"
#define SD_CS         4
#define ETH_CS        10
#define GREEN_LED     5
#define RED_LED       3

#define MAX_FIELD_SIZE 8

#define MESSAGE_DELIMITER "|"


const byte mac[] PROGMEM = { 0xB0, 0x0B, 0x1E, 0xB0, 0x0B, 0x1E };
const byte ipAddr[] PROGMEM = {192, 168, 1, 222};
IPAddress server(192, 168, 1, 204);


EthernetClient net;
PubSubClient client(net);

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission

char tempChars[(MAX_FIELD_SIZE * 3) + 3];

char deviceName[MAX_FIELD_SIZE] = {0};
char messageType[MAX_FIELD_SIZE] = {0};
char messageValue[MAX_FIELD_SIZE] = {0};

// The encryption key has to be the same as the one in the server
const uint8_t key[] PROGMEM = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                              };


void setup()
{
  Serial.begin(9600);
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  pinMode(ETH_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  //  digitalWrite(ETH_CS, HIGH);

  


  initEthernet();
  initMqtt();
  sendRetainedMessage("bridge/status", "initialized");
  initRadio();
  
}

void loop() {
  digitalWrite(GREEN_LED, HIGH);
  if (rf69.available()) {
    Serial.println("message recieved");
    // Should be a message for us now
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (!len) return;
      buf[len] = 0;
      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);

      parseData((char*)buf);
      showParsedData();

      digitalWrite(GREEN_LED, LOW);
      Blink(RED_LED, 100, 3);
      Serial.println("Sending message to MQTT");
      sendMessage();

      Blink(GREEN_LED, 100, 3);
    } else {
      Serial.println("Receive failed");
    }
  }
}

void enableRadio() {
  Serial.println("Enabling Radio");
  digitalWrite(SD_CS, HIGH);
  digitalWrite(ETH_CS, HIGH);
  digitalWrite(RFM69_CS, LOW);
}

void enableEthernet() {
  Serial.println("Enabling Ethernet");
  digitalWrite(SD_CS, HIGH);
  digitalWrite(RFM69_CS, HIGH);
  digitalWrite(ETH_CS, LOW);
}

void initEthernet() {
  enableEthernet();
  Ethernet.begin(mac, ipAddr);
}

void initMqtt() {
  client.setServer(server, 1883);
}

void sendRetainedMessage(char* topic, char* message) {
  enableEthernet();

  if (!client.connected()) {
    connectMqtt();
  }

  Serial.print("Publishing to topic: ");
  Serial.print(topic);
  Serial.print(" Message: ");
  Serial.println(message);
  client.publish(topic, message, true);

  client.loop();

  enableRadio();
}

void sendMessage()
{
  //build the topic name
  char buff[sizeof(deviceName) + sizeof(messageType) + 1];

  strcpy(buff, deviceName);
  strcat(buff, "/");
  strcat(buff, messageType);

  enableEthernet();

  if (!client.connected()) {
    connectMqtt();
  }

  Serial.print("Publishing to topic: ");
  Serial.print(buff);
  Serial.print(" Message: ");
  Serial.println(messageValue);
  client.publish(buff, messageValue);

  client.loop();

  enableRadio();
}

void connectMqtt() {
  Serial.print("connecting...");
  int connCount = 0;

  while (!client.connect("rfm69bridge")) {
    Serial.print(".");
    delay(1000);
    if (connCount >= 10) {
      while (true) {
        Serial.println();
        Serial.print("MQTT failure (retry count > 10)");
        Blink(GREEN_LED, 500, 1);
      }
    }
  }

  Serial.println("\nconnected!");
}


void initRadio() {
  digitalWrite(RFM69_RST, LOW);

  resetRadio();
  enableRadio();

  Serial.println("initializing radio...");
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

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(14 , true);  // range from 14-20 for power, 2nd arg must be true for 69HCW


  rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

void resetRadio()
{
  Serial.println("Radio reset");
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
}

void parseData(char* buf) {

  // split the data into its parts

  //is it okay to copy like this over and over?
  strcpy(tempChars, buf);
  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, MESSAGE_DELIMITER);     // get the first part - the string
  strcpy(deviceName, strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, MESSAGE_DELIMITER); // this continues where the previous call left off
  strcpy(messageType, strtokIndx);

  strtokIndx = strtok(NULL, MESSAGE_DELIMITER);
  strcpy(messageValue, strtokIndx);

}

void showParsedData() {
  Serial.print("Device Name: ");
  Serial.println(deviceName);
  Serial.print("Message Type: ");
  Serial.println(messageType);
  Serial.print("Message Value: ");
  Serial.println(messageValue);
}

void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i = 0; i < loops; i++)  {
    digitalWrite(PIN, HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN, LOW);
    delay(DELAY_MS);
  }
}
