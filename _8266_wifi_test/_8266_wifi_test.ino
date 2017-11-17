#include <dummy.h>


#include "WiFiConstants.h"
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define ACTIVATED LOW
#define DEACTIVATED HIGH

const int POWER_PIN = 3; // GPIO2
const int CASE_PIN = 2; // GPIO2
//const int TX_PIN = 2; 
//const int GPIO0 = D3;

// Connect to the WiFi
//SSID and PASSWORD are defined in WifiConstants.h, which isn't checked into GitHub
const char* ssid = WIFI_SSID; //add your SSID here
const char* password = WIFI_PASSWORD; //add your password here
const char* mqtt_server = "192.168.1.202";
const char* mqtt_user = "powerswitch";
const char* mqtt_password = "p0w3r!";
const char* monitor_topic = "power/esptest/#";
const char* log_topic = "power/esptest/log";
const char* state_topic = "power/esptest/state";
const char* power_topic = "power/esptest/control";
const char* all_power_topic = "power/all/control";
const char* will_message = "disconnected";

int counter = 0;


unsigned long logTime = 0;
int pcState = 0;



WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
  }
  if (strcmp(power_topic, topic) == 0 || strcmp(all_power_topic, topic) == 0) {
    pressPowerButton();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect("esptest ESP8266 Client", mqtt_user, mqtt_password, log_topic, 0, 0, will_message)) {
      Serial.println("mqtt connected");
      initLogQueue();
      client.subscribe(power_topic);
      client.subscribe(all_power_topic);

    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_wifi() {

  delay(10);
  WiFi.hostname("esptest");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("wifi connected"); 
}


void setup()
{
  Serial.begin(115200);
  setup_wifi();
  pinMode(CASE_PIN, OUTPUT);
  pinMode(POWER_PIN, INPUT);
//  pinMode(TX_PIN, OUTPUT); 
  digitalWrite(CASE_PIN, HIGH);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  logTime = millis();
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  checkPcState();
}


void initLogQueue() {
  client.publish(log_topic, "initialized", 0);
}

void pressPowerButton() {
  digitalWrite(CASE_PIN, LOW);
  delay(1000);
  digitalWrite(CASE_PIN, HIGH);
  client.publish(log_topic, "power button pressed", 0);
}

void checkPcState()
{
  unsigned long currentTime = millis();
  if (currentTime > (logTime + 1000)) {
    int currentPcState = digitalRead(POWER_PIN);
    if (currentPcState != pcState)
    {
      pcState = currentPcState;
      char payload[10];
      sprintf(payload, "%d", pcState);
      client.publish(log_topic, "updated PC power status", 0);
      client.publish(state_topic, payload, 1);

    }
    logTime = currentTime;
  }
}



