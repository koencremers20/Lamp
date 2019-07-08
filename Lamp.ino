#include <WiFi.h>
#include <PubSubClient.h>
#include "credentials.h"
#include "Ledstrip.h"

#define DEBUG_PRINT 1

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 10;

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;
const char* mqtt_server = MQTT_SERVER;
uint8_t i = 1;
WiFiClient espClient;
PubSubClient client(espClient);

int red, green, blue = 0;

enum Scenes {
  Solid,
  Goodnight,
  Evening
};

Scenes currentScene = Goodnight;

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

void setup() {
  pinMode(2, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  if (DEBUG_PRINT) {
    Serial.begin(115200);
  }
  // Disable SD SPI
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  setupLedstrip();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (!client.connected()) {
      reconnect();
    }

    switch (currentScene)
    {
      case Solid:
        setRGBLedstrip(red,green,blue);
        break;
      case Goodnight:
        powerOffLedstrip();
        break;
      case Evening:
        setRGBLedstrip(55, 55, 55);
        break;
    }
    //    setRedValue(5, i);
    //    setGreenValue(6, i);
    //    setBlueValue(7, i);
    
    //    i += 5;


    client.loop();
  }
}



//*********************************************************
//                         MQTT
//*********************************************************

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("scenes");
      client.subscribe("brightness");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
//  Serial.println();
//  Serial.println(message);
  if (!strcmp(message.c_str(), "Solid color")) {
    client.subscribe("Color");
    Serial.println("Solid Color");
    currentScene = Solid;
  }
  if (!strcmp(message.c_str(), "Evening")) {
    client.unsubscribe("Color");
    Serial.println("Evening");
    currentScene = Evening;
  }
  if (!strcmp(topic, "Color")) {
    sscanf(message.c_str(), "%d %d %d", &red, &green, &blue);
    Serial.print(red);
    Serial.print(" ");
    Serial.print(green);
    Serial.print(" ");
    Serial.println(green);
  }
  if (!strcmp(message.c_str(), "Good night")) {
    client.unsubscribe("Color");
    Serial.println("Good night");
    currentScene = Goodnight;
  }
}
