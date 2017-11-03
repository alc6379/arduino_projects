#include <dummy.h>


#include "WiFiConstants.h"
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define ACTIVATED LOW
#define DEACTIVATED HIGH

// Connect to the WiFi
//SSID and PASSWORD are defined in WifiConstants.h, which isn't checked into GitHub
const char* ssid = WIFI_SSID; //add your SSID here
const char* password = WIFI_PASSWORD; //add your password here
const char* mqtt_server = "192.168.1.202";
const char* mqtt_user = "powerswitch";
const char* mqtt_password = "p0w3r!";
const char* monitor_topic = "power/wemos/#";
const char* log_topic = "power/wemos/log";
const char* state_topic = "power/wemos/state";
const char* power_topic = "power/wemos/control";
const char* all_power_topic = "power/all/control";

int counter = 0;

int buttonState = DEACTIVATED;
unsigned long logTime = 0;
int pcState = 0;



WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);
  }
  Serial.println();
  if (strcmp(power_topic, topic) == 0 || strcmp(all_power_topic, topic) == 0) {
    Serial.println("evaluating shutdown condition");
    Serial.println("recieved shutdown command from MQTT");
    pressPowerButton();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("desktop ESP8266 Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(power_topic);
      client.subscribe(all_power_topic);
      initLogQueue();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, OUTPUT);
  pinMode(D5, INPUT);
  digitalWrite(D3, HIGH);
  logTime = millis();
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  checkButtonState();
  checkPcState();
}

// this won't be needed once we put this on a small 8266 board
void checkButtonState() {
  int buttonPress = digitalRead(D2);
  if (buttonPress == ACTIVATED)
  {
    if (buttonState == DEACTIVATED) {
      pressPowerButton();
      buttonState = ACTIVATED;

    }
  } else {
    buttonState = DEACTIVATED;
  }
}

void initLogQueue() {
  Serial.println("log_topic initialized");
  client.publish(log_topic, "initialized", 0);
}

void pressPowerButton() {
  digitalWrite(D3, LOW);
  delay(200);
  digitalWrite(D3, HIGH);
  char payload[10];
  sprintf(payload, "%d", counter);
  Serial.println("incrementing log topic");
  client.publish(log_topic, "power button pressed", 0);
}

void checkPcState()
{
  unsigned long currentTime = millis();
  if (currentTime > (logTime + 1000)) {
    int currentPcState = digitalRead(D5); //Need to get this to D4 when we move to small 8266 chips.
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



