#include "WiFiConstants.h"
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

//const int POWER_PIN = 3; // TX/GPIO1
int POWER_PIN = -1; // only for use with Wemos D1 prototype
int CASE_PIN = -1; // GPIO2

int configuring = 0;

//SSID and PASSWORD are defined in WifiConstants.h, which isn't checked into GitHub
const char* ssid = WIFI_SSID; //add your SSID here
const char* password = WIFI_PASSWORD; //add your password here
const char* mqtt_server = MQTT_SERVER;
const char* mqtt_user = MQTT_USER;
const char* mqtt_password = MQTT_PASSWORD;
const char* will_message = "disconnected";

char* log_topic;
char* state_topic;
char* power_topic = "";
char* monitor_topic;
char* all_power_topic = "";

String configTopic;
String clientName;
String macAddress;

int counter = 0;

unsigned long logTime = 0;
unsigned long statusTime = 0;
volatile int pcState = -1;
int currentPcState = -1;

StaticJsonBuffer<384> jsonBuffer;
WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  Serial.println(D2);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  logTime = millis();
  statusTime = logTime;
}

void loop()
{
  if (!client.connected()) {
    Serial.println("client not connected");
    reconnect();
  }


  if (POWER_PIN != -1) {
    if (pcState != currentPcState) {
      checkPcState();
    }
  }

  //report status on the serial monitor for debugging every 30 seconds
  if ((long)( millis() - statusTime ) >= 0)
  {
    Serial.print("statusTime: ");
    Serial.print(statusTime);
    Serial.println();
    String statusMessage = String("statusTime: ") + String(statusTime);

    statusTime += 30000;
  }

  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String strTopic = String(topic);
  if (configTopic.compareTo(strTopic) == 0 && configuring == 0) {
    configureModule(payload);
  }
  if (strTopic.compareTo(String(power_topic)) == 0 || strTopic.compareTo(String(all_power_topic)) == 0) {
    pressPowerButton();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect((char*)macAddress.c_str(), mqtt_user, mqtt_password, log_topic, 0, 0, will_message)) {
      Serial.println("mqtt connected");
      client.subscribe((char*)configTopic.c_str());
      client.publish("config-request", (char*)macAddress.c_str(), 1);
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_wifi() {
  delay(10);
  macAddress = WiFi.macAddress();
  String hostname = String("esp-") + macAddress;
  WiFi.hostname(hostname);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("wifi connected");

  setConfigTopic(macAddress);
  Serial.println(macAddress);
}

void initLogQueue() {
  client.publish(log_topic, "initialized", 0);
}

void setConfigTopic(String mac) {
  configTopic = String("config/") + mac;
}

void configureModule(byte* inputPayload) {

  configuring = 1;

  unsigned char *chrPayload = inputPayload;
  Serial.println(String((char*)chrPayload));

  JsonObject& root = jsonBuffer.parseObject(chrPayload);

  if (root.success()) {

    log_topic = (char*)root["log-topic"].as<char*>();
    Serial.print("log-topic: ");
    Serial.println(log_topic );
    client.publish(log_topic, "initializing...", 1);


    state_topic = (char*)root["state-topic"].as<char*>();
    Serial.print("state-topic: ");
    Serial.println(state_topic );

    client.loop();
    client.unsubscribe(power_topic);
    power_topic = (char*)root["power-topic"].as<char*>();
    client.subscribe(power_topic, 1);
    Serial.print("power-topic: ");
    Serial.println(power_topic );

    client.unsubscribe(all_power_topic);
    all_power_topic = (char*)root["all-power-topic"].as<char*>();
    client.subscribe(all_power_topic, 1);
    Serial.print("all-power-topic: ");
    Serial.println(all_power_topic );

    POWER_PIN = root["power-pin"];
    Serial.print("power-pin: ");
    Serial.print(POWER_PIN);
    Serial.println();
    //    POWER_PIN = 7;
    if (POWER_PIN != -1)
    {
      pinMode(POWER_PIN, INPUT);
      //set the PC power read state
      pcState = digitalRead(POWER_PIN);
      attachInterrupt(POWER_PIN, powerStateChange, CHANGE);
    }


    CASE_PIN = root["case-pin"];
    //    CASE_PIN = 3;
    Serial.print("case-pin: ");
    Serial.print(CASE_PIN);
    Serial.println();

    //    Serial.end();
    pinMode(CASE_PIN, OUTPUT);
    digitalWrite(CASE_PIN, HIGH);

    initLogQueue();
    configuring = 0;
  }
}

void pressPowerButton() {
  Serial.println("recieved power signal");
  digitalWrite(CASE_PIN, LOW);
  delay(1000);
  digitalWrite(CASE_PIN, HIGH);
  client.publish(log_topic, "power button pressed", 1);
}

void checkPcState()
{
  Serial.print("updating power state ");
  Serial.print(pcState);
  Serial.println();
  currentPcState = pcState;
  char payload[10];
  sprintf(payload, "%d", pcState);
  client.publish(log_topic, "updated PC power status", 1);
  client.publish(state_topic, payload, 1);
}

void powerStateChange() {
  pcState = digitalRead(POWER_PIN);
}


