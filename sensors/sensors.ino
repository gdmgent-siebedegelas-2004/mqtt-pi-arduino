#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include "Arduino_SensorKit.h"
#include "wifiSecret.h"

#define MOISTURE_SENSOR_PIN A0       // Pin waarop de vochtsensor is aangesloten
#define LIGHT_SENSOR_PIN A1          // Pin waarop de lichtsensor is aangesloten
#define Environment Environment_I2C  // Pin waarop de Humidity & temperatuursensor is aangesloten

#define INTERVAL 1000           // Tijd tussen metingen in milliseconden (1 seconde)

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.50.61";
int port = 1883;
const char moistureTopic[] = "arduino/inputmoisture";
const char publishTopic[] = "arduino/test";

int InputMoisterValue = 200; // Default moisture threshold value

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Environment.begin();
  pinMode(13, INPUT_PULLUP);
  pinMode(12, OUTPUT);

  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }
  Serial.println("You're connected to the network");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
  while (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    delay(5000);
  }
  Serial.println("You're connected to the MQTT broker!");
  mqttClient.onMessage(onMqttMessage);

  mqttClient.subscribe(moistureTopic);
}

void reconnect() {
  // Serial.print("Attempting to connect to WPA SSID: ");
  // Serial.println(ssid);
  //  WiFi.end();

  //   while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
  //   Serial.print(".");
  //   delay(5000);
  // }
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
   
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(broker, port)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
        mqttClient.subscribe(moistureTopic);

    } else {
      Serial.print("failed, rc=");
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
    if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.poll();
  // Serial.println(mqttClient.messageTopic());
  // Serial.println(mqttClient.read());
  // if (mqttClient.parseMessage()) {
  //   Serial.println("message");
  //   if (mqttClient.messageTopic() == moistureTopic) {
  //     InputMoisterValue = mqttClient.readString().toInt();
  //     Serial.print("New Moisture Threshold: ");
  //     Serial.println(InputMoisterValue);
  //   } 
  // }

  int moistureValue = analogRead(MOISTURE_SENSOR_PIN);  // Lees de vochtigheidswaarde
  int lightValue = analogRead(LIGHT_SENSOR_PIN);        // Lees de lichtwaarde
  int waterniveau = digitalRead(13);                    // Lees het waterniveau
  int temp = Environment.readTemperature();             // Lees de temperatuur
  int humid = Environment.readHumidity();               // Lees de vochtigheid

  // Serial.print("Vochtigheid: ");
  // Serial.println(moistureValue);
  mqttClient.beginMessage("arduino/vochtigheid");
  mqttClient.print(moistureValue);
  mqttClient.endMessage();

  Serial.print("Lichtwaarde: ");
  Serial.println(lightValue);
  mqttClient.beginMessage("arduino/lichtwaarde");
  mqttClient.print(lightValue);
  mqttClient.endMessage();

  // Serial.print("Waterniveau: ");
  // Serial.println(waterniveau);
  mqttClient.beginMessage("arduino/waterniveau");
  mqttClient.print(waterniveau);
  mqttClient.endMessage();

  // Serial.print("Temperature: ");
  // Serial.print(temp);
  // Serial.println(" C");
  // Serial.print("Humidity: ");
  // Serial.print(humid);
  // Serial.println(" %");
  mqttClient.beginMessage("arduino/temperature");
  mqttClient.print(temp);
  mqttClient.endMessage();
  mqttClient.beginMessage("arduino/humidity");
  mqttClient.print(humid);
  mqttClient.endMessage();

  if (waterniveau == LOW) {
    Serial.println("REFILL RESEVOIR!");
    digitalWrite(12, LOW);
  } else {
    if (moistureValue <= InputMoisterValue) {
      Serial.println("Adding water TO PLANT!");
      digitalWrite(12, HIGH);
      delay(2000);
      digitalWrite(12, LOW);
      delay(5000);
    } else {
      Serial.println("grond is vochtig :)");
      digitalWrite(12, LOW);
    }
  }

  // Serial.println(InputMoisterValue);
  // Serial.println(" ");
  delay(INTERVAL);

  
}


void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  String value = "";
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    // Serial.print((char)mqttClient.read());
    value += (char)mqttClient.read();
  }
  Serial.println(value);
  InputMoisterValue = value.toInt();

  Serial.println();
  Serial.println();
}
