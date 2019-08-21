#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "DHTesp.h"

DHTesp dht;

//#define D6 12
#define DHTPIN D6     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT11   // there are multiple kinds of DHT sensors
#define MQTT_MAX_PACKET_SIZE 1024

const char* mqttServer = "10.0.0.221";

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "shut up";
const char* password = "andgoaway";

int timeSinceLastRead = 0;
int startMillis;
int currentMillis;
int samplingPeriod;

char tempF[50];
char humidity[50];
char heatIndex[50];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  dht.setup(DHTPIN, DHTesp::DHT11); // Connect DHT sensor to GPIO 17
  WiFi.begin(ssid, password);
  //  pinMode(DHTPIN, INPUT_PULLUP);
  //  pinMode(LED_BUILTIN, OUTPUT);

  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to the WiFi network");

  client.setServer(mqttServer, 1883);
  reconnect();
  client.publish("bedroom/tempStatus", "init");
  //  client.setCallback(callback);
  //
  //  digitalWrite(LED_BUILTIN, HIGH);

  startMillis = millis();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266 Client")) {
      Serial.println("connected");
      //      // ... and subscribe to topic
      //      client.subscribe("desktop/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  currentMillis = millis();
  // Report every 2 seconds.
  //get the minimum sampling period
  samplingPeriod = dht.getMinimumSamplingPeriod();
  if (currentMillis - startMillis >= ( 2000 + samplingPeriod)) {
    if (!client.connected()) {
      reconnect();

      client.loop();
      //      delay(dht.getMinimumSamplingPeriod());
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.getHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.getTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float f = dht.toFahrenheit(t);

      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        timeSinceLastRead = 0;
        return;
      }

      // Compute heat index in Fahrenheit (the default)
      float hif = dht.computeHeatIndex(f, h, true);
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);

      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");
      Serial.print(f);
      Serial.print(" *F\t");
      Serial.print("Heat index: ");
      Serial.print(hic);
      Serial.print(" *C ");
      Serial.print(hif);
      Serial.println(" *F");

      timeSinceLastRead = 0;

      snprintf(tempF, 50, "%.2f", f);
      snprintf(heatIndex, 50, "%.2f", hif);
      snprintf(humidity, 50, "%.2f", h);

      client.publish("bedroom/temperature", tempF);
      client.loop();
      client.publish("bedroom/heatindex", heatIndex);
      client.loop();
      client.publish("bedroom/humidity", humidity);


    }
  }
}
