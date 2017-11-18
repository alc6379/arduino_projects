#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WifiConstants.h"

#define MQTT_MAX_PACKET_SIZE 384 //need to increase packet size for larger JSON bodies. 

//const char *testConfig = "{\"log-topic\" : \"device/test/log\", \"state-topic\" : \"device/test/state\", \"power-topic\": \"device/test/power\", \"all-power-topic\": \"device/all/power\", \"reset-topic\": \"device/test/config-timestamp\", \"monitor-topic\": \"device/test/monitor\", \"extra-topic\": \"extra\"}";
//const char *jsonValue = "{\"val1\": \"test\", \"val2\":\"other val\"}";
String configTopic;
String clientName;
const char* ssid = WIFI_SSID; //add your SSID here
const char* password = WIFI_PASSWORD; //add your password here
const char* mqtt_server = "192.168.1.202";
const char* mqtt_user = "powerswitch";
const char* mqtt_password = "p0w3r!";

char* log_topic;
char* state_topic;
char* power_topic;
char* monitor_topic;
char* all_power_topic;

String macAddress;

StaticJsonBuffer<512> jsonBuffer;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //  configureTopics();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void setup_wifi() {

  delay(10);
  WiFi.hostname("jsontest");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("wifi connected");
  macAddress = WiFi.macAddress();
  setConfigTopic(macAddress);
  Serial.println(macAddress);

}

void callback(char* topic, byte* payload, unsigned int length) {
  // put your setup code here, to run once:
  Serial.println("got message");
  String strTopic = String(topic);
  if (configTopic.compareTo(strTopic) == 0) {
    configureModule(payload);
  }
}

void configureModule(byte* inputPayload) {
  unsigned char *chrPayload = inputPayload;
  Serial.println(String((char*)chrPayload));

  JsonObject& root = jsonBuffer.parseObject(chrPayload);

  if (root.success()) {

    log_topic = (char*)root["log-topic"].as<char*>();
    Serial.print("log-topic: ");
    Serial.println(log_topic );

    state_topic = (char*)root["state-topic"].as<char*>();
    Serial.print("state-topic: ");
    Serial.println(state_topic );

    power_topic = (char*)root["power-topic"].as<char*>();
    Serial.print("power-topic: ");
    Serial.println(power_topic );

    monitor_topic = (char*)root["monitor-topic"].as<char*>();
    Serial.print("monitor-topic: ");
    Serial.println(monitor_topic );

    all_power_topic = (char*)root["all-power-topic"].as<char*>();
    Serial.print("all-power-topic: ");
    Serial.println(all_power_topic );
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Attempt to connect
    if (client.connect((char*) macAddress.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("mqtt connected");
      client.subscribe((char*)configTopic.c_str());
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setConfigTopic(String mac) {
  configTopic = String("config/") + mac;
}

