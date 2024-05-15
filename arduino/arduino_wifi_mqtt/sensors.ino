#include "Arduino_SensorKit.h"
//MQTT
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "wifiSecret.h"

//ARD
#define MOISTURE_SENSOR_PIN A0       // Pin waarop de vochtsensor is aangesloten
#define LIGHT_SENSOR_PIN A1          // Pin waarop de lichtsensor is aangesloten
#define Environment Environment_I2C  // Pin waarop de Humidity & temperatuursensor is aangesloten

#define INTERVAL 1000           // Tijd tussen metingen in milliseconden (1 seconde)
#define MOISTURE_THRESHOLD 200  // Drempelwaarde voor vochtigheid waarboven de pomp moet worden uitgeschakeld

//MQTT
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.50.61";
int port = 1883;
const char topic[] = "arduino/test";

//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;


void setup() {
  //ARD
  Wire.begin();
  Serial.begin(9600);
  Environment.begin();
  pinMode(13, INPUT_PULLUP);
  pinMode(12, OUTPUT);

  //MQTT
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1)
      ;
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  //ARD
  delay(2000);  // 2000 milliseconden = 2 seconden

  int moistureValue = analogRead(MOISTURE_SENSOR_PIN);  // Lees de vochtigheidswaarde
  int lightValue = analogRead(LIGHT_SENSOR_PIN);        // Lees de lichtwaarde
  int waterniveau = digitalRead(13);                    // Lees het waterniveau
  int temp = Environment.readTemperature();             // Lees de tempreture
  int humid = Environment.readHumidity();               // Lees de Humidity

  //MQTT
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  //ARD
  Serial.print("Vochtigheid: ");
  Serial.println(moistureValue);
  //MQTT
  mqttClient.beginMessage("arduino/vochtigheid");
  mqttClient.print(moistureValue);
  mqttClient.endMessage();

  //ARD
  Serial.print("Lichtwaarde: ");
  Serial.println(lightValue);
  //MQTT
  mqttClient.beginMessage("arduino/lichtwaarde");
  mqttClient.print(lightValue);
  mqttClient.endMessage();

  //ARD
  Serial.print("Waterniveau: ");
  Serial.println(waterniveau);
  //MQTT
  mqttClient.beginMessage("arduino/waterniveau");
  mqttClient.print(waterniveau);
  mqttClient.endMessage();

  //ARD
  Serial.print("Temperature: ");
  Serial.print(Environment.readTemperature());  //print temperature
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(Environment.readHumidity());  //print humidity
  Serial.println(" %");
  //MQTT
  mqttClient.beginMessage("arduino/temperature");
  mqttClient.print(temp);
  mqttClient.endMessage();
  mqttClient.beginMessage("arduino/humidity");
  mqttClient.print(humid);
  mqttClient.endMessage();

  //ARD/MQTT
//ARD/MQTT
  if (waterniveau == LOW) {


    Serial.println("REFILL RESEVOIR!");


      digitalWrite(12, LOW);

  } else {
    //onder 300 droog
    //boven 500 nat
    if (moistureValue <= 200) {
      Serial.println("Adding water TO PLANT!");

      digitalWrite(12, HIGH);
      delay(3000);
      digitalWrite(12, LOW);
    } else {
      Serial.println("grond is vochtig 🙂");

      digitalWrite(12, LOW);
    }
  }


  Serial.println(" ");




  delay(INTERVAL);  // Wacht voor de volgende meting
}