#include <WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include <EEPROM.h>
#include "credentials.h"

#define EEPROM_SIZE 1
#define DEBUG_PRINT 1

// How many leds in your strip?
#define NUM_LEDS 240
#define DATA_PIN 13
// Define the array of leds
CRGB leds[NUM_LEDS];

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 10;

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;
const char* mqtt_server = MQTT_SERVER;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
boolean power = false;
static uint8_t hue = 0;
int i = 0;

boolean forward = false;

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

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  Serial.println("Message arrived");
   Serial.print(topic);
    Serial.print("] ");
  for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
   Serial.println();
  Serial.println(message);
  if (!strcmp(message.c_str(), "1")) {
    power = true;
    EEPROM.write(0, power);
  }
  if (!strcmp(message.c_str(), "0")) {
    power = false;
    EEPROM.write(0, power);
  }
  if (!strcmp(topic, "color")) Serial.println("color");

  EEPROM.commit();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect("client")) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("power");
      client.subscribe("color");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      power = false;
      for (int j = 0; j < NUM_LEDS; j++) {
        leds[j] = CHSV(0, 0, 0);
      }
      FastLED.show();
      ESP.restart();
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
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

  LEDS.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  LEDS.setBrightness(50);

  EEPROM.begin(EEPROM_SIZE);
  power = EEPROM.read(0);
  power ? Serial.println("AAAAAAN") : Serial.println("UUUIIIITTTT");
}

void fadeall() {
  for (int j = 0; j < NUM_LEDS; j++) {
    leds[j].nscale8(250);
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    fadeall();
    if (power)
    {
      leds[i] = CHSV(hue++, 255, 255);
    }
    else
    {
      leds[i] = CHSV(0, 0, 0);
    }
    FastLED.show();

    if (forward) i--;
    else i++;

    if (i >= NUM_LEDS) forward = true;
    if (i <= 0) forward = false;
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }

}
