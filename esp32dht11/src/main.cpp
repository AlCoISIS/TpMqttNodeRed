/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
  Modified by Alexis Combes
*********/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// TODO : Pour compiler ce code, vous devez installer la library "DHT sensor library" de "Adafruit" dans votre IDE.
#include "DHT.h"
// Definit la broche de l'Arduino sur laquelle la
// broche DATA du capteur est reliee
#define DHTPIN 25
// Definit le type de capteur utilise
#define DHTTYPE DHT11
// Declare un objet de type DHT
// Il faut passer en parametre du constructeur
// de l'objet la broche et le type de capteur
DHT dht(DHTPIN, DHTTYPE);

// TODO : Replace the next variables with your SSID/Password combination
const char *ssid = "xxxx";
const char *password = "xxxx";

// TODO : Add your MQTT Broker IP address, example:
// const char* mqtt_server = "192.168.1.144";
const char *mqtt_server = "XXX.XXX.XXX.XXX";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// LED Pin
const int ledPin = 4;

void setup_wifi();
void callback(char *topic, byte *message, unsigned int length);

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);

  dht.begin();
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client_DHT"))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000)
  {
    lastMsg = now;

    // Read DHT 11 temperature and humidity, and publish them to MQTT
    String dhtHumidity = String(dht.readHumidity());
    Serial.print("DHT Humidity: ");
    Serial.println(dhtHumidity);
    String dhtTemperature = String(dht.readTemperature());
    Serial.print("DHT Temperature: ");
    Serial.println(dhtTemperature);
    client.publish("esp32/DHT11/temperature", dhtTemperature.c_str());
    client.publish("esp32/DHT11/humidity", dhtHumidity.c_str());
  }
}
