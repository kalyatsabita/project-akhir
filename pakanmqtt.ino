#include "RTClib.h"
#include <ESP32Servo.h>
#include "HX711.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define ssid "Hai"
#define password "20012003"
#define mqtt_server "192.168.43.7"
#define mqtt_user ""
#define mqtt_password ""

#define mqtt_topic "makan"
//#define servo_topic "move"

WiFiClient espClient;
PubSubClient client(espClient);
Servo myservo;
RTC_DS3231 rtc;
HX711 scale;

#define PIN_SG90 13
#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 4
#define calibration_factor 400

//int pos = 0;
int ringhour1 = 19;
int ringmi1 = 40;
int ringhour2 = 20;
int ringmi2 = 05;
int ringhour3 = 20;
int ringmi3 = 20;

const int pakan_cukup = 30;
//const int pakan_cukup2 = 40;
//const int pakan_cukup3 = 50;
//const int pakan_cukup4 = 60;
const int pakan_banyak = 50;
//const int pakan_banyak2 = 60;
//const int pakan_banyak3 = 70;
//const int pakan_banyak4 = 80;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  connect_wifi();
  client.setCallback(callback);
  client.setServer(mqtt_server, 1883);
  myservo.setPeriodHertz(50); // PWM frequency for SG90
  myservo.attach(PIN_SG90);
  rtc.begin();
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
      reconnect();
    }
    client.loop();
  
  DateTime now = rtc.now();
  printDateTime(now);

  int pakan = loadCell(); // Read the weight of the food
  Serial.print("Current weight: ");
  Serial.println(pakan);

  // Publish the weight to the MQTT topic
  client.publish(mqtt_topic, String(pakan).c_str());
  
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
}

void connect_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print("Connecting to AP ... ");    
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED ){    
    delay(500);
    Serial.print(".");    
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)){
      Serial.println("Connected");  
      client.subscribe("makan");
//      client.subscribe("move");
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
//  int pos = 0;
  Serial.print("Feed time, pakan: ");
  Serial.println(pakan);

  if (pakan == 0) {
    Serial.println("Kondisi Pakan : Kosong");
//    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//      // in steps of 1 degree
//      myservo.write(pos);              // tell servo to go to position in variable 'pos'
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }
//    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//      myservo.write(pos);              // tell servo to go to position in variable 'pos'
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }
    moveServo(180); // Turn on
    delay(10000); // Keep it on for 5 seconds
    moveServo(0); // Turn off
  } else if (pakan > 0 && pakan <= pakan_cukup) {
    Serial.println("Kondisi Pakan : Sedikit");
//    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//      // in steps of 1 degree
//      myservo.write(pos);              // tell servo to go to position in variable 'pos'
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }
//    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//      myservo.write(pos);              // tell servo to go to position in variable 'pos'
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }
    moveServo(180); // Turn on
    delay(10000); // Keep it on for 5 seconds
    moveServo(0); // Turn off
  } else if (pakan > pakan_cukup && pakan <= pakan_banyak) {
    Serial.println("Kondisi Pakan : Cukup");
//   for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
//      // in steps of 1 degree
//      myservo.write(pos);              // tell servo to go to position in variable 'pos'
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }
//    for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//      myservo.write(pos);              // tell servo to go to position in variable 'pos'
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }
    moveServo(180); // Turn on
    delay(5000); // Keep it on for 5 seconds
    moveServo(0); // Turn off
  } else if (pakan > pakan_banyak) {
    Serial.println("Kondisi Pakan : Banyak");
//   for (pos = 0; pos <= 0; pos += 1) { // goes from 0 degrees to 180 degrees
//      // in steps of 1 degree
//      myservo.write(pos);              // tell servo to go to position in variable 'pos'
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }
//    for (pos = 0; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//      myservo.write(pos);              // tell servo to go to position in variable 'pos'
//      delay(15);                       // waits 15ms for the servo to reach the position
//    }
    moveServo(0); // Turn on
  } 
}

void moveServo(int angle) {
  myservo.write(angle);
}

int loadCell() {
  int GRAM = scale.get_units();
  Serial.println(GRAM);
  return GRAM;
}
