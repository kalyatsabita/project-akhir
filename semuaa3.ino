#include <WiFi.h>
#include <PubSubClient.h>
#include "RTClib.h"
#include <ESP32Servo.h>
#include "HX711.h"

// WiFi credentials
#define ssid "Hai"
#define password "20012003"

// MQTT server details
#define mqtt_server "192.168.43.7"
#define mqtt_user ""
#define mqtt_password ""

// MQTT topics
#define mqtt_topic_pakan "makan"
#define mqtt_topic_minum "minum"

// Servo pins
#define PIN_SG90 13

// Load cell pins
#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 4
#define calibration_factor 700

// Feeding Schedule
int pos = 0;
int ringhour1 = 14;
int ringmi1 = 35;
int ringhour2 = 14;
int ringmi2 = 45;
int ringhour3 = 14;
int ringmi3 = 55;

// Pakan Levels
const int pakan_cukup = 30;
//const int pakan_cukup2 = 40;
//const int pakan_cukup3 = 50;
//const int pakan_cukup4 = 60;
const int pakan_banyak = 50;
//const int pakan_banyak2 = 60;
//const int pakan_banyak3 = 70;
//const int pakan_banyak4 = 80;

// Water sensor pins
#define RELAY 18
#define POWER 33
#define SIGNAL 32

// Water Levels
const int air_sedang = 300;
const int air_tinggi = 500;
int sensorValue = 0;
float faktor_konversi = 0.4;

WiFiClient espClient;
PubSubClient client(espClient);
Servo myservo;
RTC_DS3231 rtc;
HX711 scale;

void connect_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
void printDateTime(DateTime now);
void feedPakan(int pakan);
int loadCell();
int waterSensor();
float getWaterVolume(int sensorValue);
void controlPump(bool state);
void moveServo(int angle);

void setup() {
  Serial.begin(115200);
  // Setup WiFi
  connect_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Setup servo
  myservo.setPeriodHertz(50); // PWM frequency for SG90
  myservo.attach(PIN_SG90);
  
  // Setup RTC
  rtc.begin();

  // Setup scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();

  // Setup Minum
  pinMode(RELAY, OUTPUT);
  pinMode(POWER, OUTPUT);
  digitalWrite(POWER, LOW);
  digitalWrite(RELAY, LOW);
  
  Serial.println("Setup completed");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  DateTime now = rtc.now();
  printDateTime(now);

  int pakan = loadCell();
  Serial.print("Current weight: ");
  Serial.println(pakan);

  client.publish(mqtt_topic_pakan, String(pakan).c_str());
  
   if ((ringhour1 == now.hour() && ringmi1 == now.minute())) {
    Serial.println("Waktu Makan Pagi");
    int pakan = loadCell();
    if (pakan >= 0) {
      feedPakan(pakan);
    }
  } else if ((ringhour2 == now.hour() && ringmi2 == now.minute())) {
    Serial.println("Waktu Makan Siang");
    int pakan = loadCell();
    if (pakan >= 0) {
      feedPakan(pakan);
    }
  } else if ((ringhour3 == now.hour() && ringmi3 == now.minute())) {
    Serial.println("Waktu Makan Malam");
    int pakan = loadCell();
    if (pakan >= 0) {
      feedPakan(pakan);
    }
  }
  delay(1000);

  sensorValue = waterSensor();
  float volume = getWaterVolume(sensorValue);
  Serial.print("Nilai Sensor: ");
  Serial.print(sensorValue);
  Serial.print(" -> Volume Air: ");
  Serial.print(volume);
  Serial.println(" mililiter");
//  sensorValue = waterSensor();
//  Serial.print("Water Level: ");
//  Serial.println(sensorValue);

  if (volume == 0) {
    Serial.println("Water Level: Empty");
    controlPump(true);
  } else if (volume > 0 && volume <= air_sedang) {
    Serial.println("Water Level: Low");
    controlPump(true);
  } else if (volume > air_sedang && volume <= air_tinggi) {
    Serial.println("Water Level: Medium");
    controlPump(true);
    delay(500);
    controlPump(false);
  } else if (volume > air_tinggi) {
    Serial.println("Water Level: High");
    controlPump(false);
  }

  client.publish(mqtt_topic_minum, String(volume).c_str());
  delay(1000); // Delay between sensor readings
}

void connect_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
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

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected");
      client.subscribe(mqtt_topic_pakan);
      client.subscribe(mqtt_topic_minum);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
}

void printDateTime(DateTime now) {
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

void feedPakan(int pakan) {
  Serial.print("Feed time, pakan: ");
  Serial.println(pakan);

  if (pakan == 0) {
    Serial.println("Kondisi Pakan : Kosong");
    moveServo(180); // Turn on
    delay(10000); // Keep it on for 5 seconds
    moveServo(0); // Turn off                       // waits 15ms for the servo to reach the position
  } else if (pakan > 0 && pakan <= pakan_cukup) {
    Serial.println("Kondisi Pakan : Sedikit");
    moveServo(180); // Turn on
    delay(10000); // Keep it on for 5 seconds
    moveServo(0); // Turn off
  } else if (pakan > pakan_cukup && pakan <= pakan_banyak) {
    Serial.println("Kondisi Pakan : Cukup");
    moveServo(180); // Turn on
    delay(5000); // Keep it on for 5 seconds
    moveServo(0); // Turn off
  } else if (pakan > pakan_banyak) {
    Serial.println("Kondisi Pakan : Banyak");
   moveServo(0); // Turn on
  } 
}

int loadCell() {
  int GRAM = scale.get_units();
  Serial.println(GRAM);
  return GRAM;
}

void controlPump(bool state) {
  digitalWrite(RELAY, state ? HIGH : LOW);
}

int waterSensor() {
  digitalWrite(POWER, HIGH);
  delay(10);
  int value = analogRead(SIGNAL);
  delay(10);
  digitalWrite(POWER, LOW);
  return value; 
}

float getWaterVolume(int sensorValue) {
  return (sensorValue * faktor_konversi);
}

void moveServo(int angle) {
  myservo.write(angle);
}
