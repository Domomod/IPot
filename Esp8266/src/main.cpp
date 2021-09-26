/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide/
*********/

// Import required libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Hash.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <errno.h>

#include "LLS.hpp"
#include "DigitalS.hpp"
#include "creditentials.hpp"

#define DHT_PIN D2
#define LIGHT_PIN D5
#define LIQUID_POWER_PIN D1
#define LIQUID_DATA_PIN A0

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht{DHT_PIN, DHT11};
LLS lls{LIQUID_DATA_PIN, LIQUID_POWER_PIN};
DigitalS lightSensor{LIGHT_PIN};
// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
float w = 0.0;
float l = 0.0;

void setup_wifi()
{
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
}

void introduce()
{
    /*
     * Introduce esp8266 to raspberry-pi server
     */
    int size = 0;
    char json_buffer[64];
    const char* format = 
    "{"
    "\"clientMac\":\"%s\""
    "}";

    size = sprintf (json_buffer, format, WiFi.macAddress().c_str());
    json_buffer[size] = '\0'; // Null terminate
    Serial.println("Introducing to server with message:");
    Serial.println(json_buffer);
    client.publish(introduction_topic, json_buffer);
}

void mqtt_callback(char *topic, byte *payload, unsigned int length) {
    Serial.printf("Message arrived | topic: %s | message : %.*s\n", topic, length, (char*) payload);
    Serial.println("-----------------------------------------------------------------------");
    if(strcmp(topic, server_status_topic) == 0)
    { // Server status topic is used to announce server start and restarts
      introduce();
    }
}

void setup_mqtt()
{
  client.setServer(mqtt_broker, mqtt_port);

  while(!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("Client %s connects to mqtt-broker\n", client_id.c_str());
    int fail = !client.connect(client_id.c_str()); 
    if (fail) {
        Serial.printf("Mqtt connection failed with state %d. Retrying...\n", client.state());
        delay(2000);
    }
    Serial.println("Succesfully connected to mqtt broker");
  }
  client.subscribe(server_status_topic);
  client.setCallback(mqtt_callback);

  introduce();
}

void setup() {
  dht.begin();
  lls.begin();
  lightSensor.begin();
  Serial.begin(115200);                  
  Serial.print("Hello World\n");
  setup_wifi();
  setup_mqtt();
}

int readSensor(String sensor_name, String log_info, float &value_ref, std::function<float(void)> read_sensor_fnc_ptr)
{
    float read_value = read_sensor_fnc_ptr();
    if (isnan(read_value))
    {
      Serial.printf("Failed to read from %s!\n", sensor_name.c_str());
      return -EINVAL;
    }
    else
    {
      value_ref = read_value;
      Serial.printf("%s: %f\n", log_info.c_str(), value_ref);
      return 0;
    }
}

void loop() {
  static unsigned long previousMillis = 0; // will store last time DHT was updated
  const long interval = 10000;  // Updates DHT readings every 10 seconds

  client.loop();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    Serial.println("----------------------------------------------------------");
    readSensor("DHT sensor",          "Temperature",    t,  std::bind(&DHT::readTemperature, &dht, false, false));
    readSensor("DHT sensor",          "Humidity",       h,  std::bind(&DHT::readHumidity, &dht, false));
    readSensor("Liquid Level sensor", "Water level",    w,  std::bind(&LLS::readLiquidLevel, &lls));
    readSensor("Light sensor",        "Light [On/Off]",  l,  std::bind(&DigitalS::readDigital, &lightSensor));

    int size = 0;
    char json_buffer[255];
    const char* format = 
    "{"
    "\"temp\":%.1f,"
    "\"humidity\":%.1f,"
    "\"water\":%.4f,"
    "\"light\":%.0f"
    "}";

    size = sprintf (json_buffer, format, t, h, w, l);
    json_buffer[size] = '\0'; // Null terminate
    Serial.println(json_buffer);
    client.publish(WiFi.macAddress().c_str(), json_buffer);
  }
}