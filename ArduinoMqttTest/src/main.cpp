#include <Arduino.h>
#include <pubsubclient.h>
#include <WiFi.h>

// WiFi settings
// TODO : Replace with your WiFi credentials here
const char *ssid = "RedmiNote11";
const char *password = "gnkk6472";

// MQTT Broker settings
// TODO : Update with your MQTT broker settings here if needed
const char *mqtt_broker = "broker.emqx.io";     // EMQX broker endpoint
const char *mqtt_topic1 = "dataCastres/topic1"; // MQTT topic
const char *mqtt_topic2 = "dataCastres/topic2"; // MQTT topic
const int mqtt_port = 1883;                     // MQTT port (TCP)
String client_id = "ArduinoClient-";
String MAC_address = "";

// Other global variables
static unsigned long lastPublishTime = 0;
WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void connectToWiFi();
void connectToMQTTBroker();
void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup()
{
  Serial.begin(9600);
  connectToWiFi();
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(mqttCallback);
  connectToMQTTBroker();
}

void printMacAddress()
{
  byte mac[6];
  Serial.print("MAC Address: ");
  WiFi.macAddress(mac);
  for (int i = 0; i < 6; i++)
  {
    MAC_address += String(mac[i], HEX);
    if (i < 5)
      MAC_address += ":";
    if (mac[i] < 16)
    {
      client_id += "0";
    }
    client_id += String(mac[i], HEX);
  }
  Serial.println(MAC_address);
}

void connectToWiFi()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  delay(3000);
  printMacAddress();
  Serial.println("Connected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void connectToMQTTBroker()
{
  while (!mqtt_client.connected())
  {
    Serial.print("Connecting to MQTT Broker as ");
    Serial.print(client_id.c_str());
    Serial.println(".....");
    if (mqtt_client.connect(client_id.c_str()))
    {
      Serial.println("Connected to MQTT broker");
      mqtt_client.subscribe(mqtt_topic1);
      mqtt_client.subscribe(mqtt_topic2);
      // Publish message upon successful connection
      String message = "Hello EMQX I'm " + client_id;
      mqtt_client.publish(mqtt_topic1, message.c_str());
    }
    else
    {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String messageTemp;
  for (int i = 0; i < length; i++)
  {
    // Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println(messageTemp);
  Serial.println("-----------------------");
  // TODO : Add your message handling logic here
  // For example, you can check the topic and perform actions based on the message content
  // Example:
  //
  // if (String(topic) == "arduino/output")
  // {
  //   Serial.print("Changing output to ");
  //   if (messageTemp == "on")
  //   {
  //     Serial.println("LED on");
  //     digitalWrite(ledPin, HIGH);
  //   }
  //   else if (messageTemp == "off")
  //   {
  //     Serial.println("LED off");
  //     digitalWrite(ledPin, LOW);
  //   }
  // }
}

void loop()
{
  if (!mqtt_client.connected())
  {
    connectToMQTTBroker();
  }
  mqtt_client.loop();
  // TODO: Add your main code here, to run repeatedly (e.g., sensor readings, publishing messages, etc. )
  // Example below : Publish a message every 10 seconds
  unsigned long currentTime = millis();
  if (currentTime - lastPublishTime >= 10000) // 10 seconds
  {
    String message = "Hello EMQX I'm " + client_id + " at " + String(currentTime / 1000) + " seconds";
    mqtt_client.publish(mqtt_topic1, message.c_str());
    Serial.println("Published message: " + message);
    lastPublishTime = currentTime;
  }
}
