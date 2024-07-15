#include "RTClib.h"
#include <ESP32Servo.h>
#include "HX711.h"

#define PIN_SG90 13
#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 4
#define calibration_factor 800

Servo myservo;
RTC_DS3231 rtc;
HX711 scale;

int pos = 0;
int ringhour1 = 19;
int ringmi1 = 20;
int ringhour2 = 19;
int ringmi2 = 25;
int ringhour3 = 19;
int ringmi3 = 30;

const int pakan_cukup = 30;
const int pakan_banyak = 50;

void setup() {
  rtc.begin();
  myservo.setPeriodHertz(50);
  myservo.attach(PIN_SG90, 500, 2400);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
  Serial.begin(115200);
}

void loop() {
  delay(1000);
  DateTime now = rtc.now();
  printDateTime(now);

  int pakan = loadCell();
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
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  } else if (pakan > 0 && pakan <= pakan_cukup) {
    Serial.println("Kondisi Pakan : Sedikit");
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  } else if (pakan > pakan_cukup && pakan <= pakan_banyak) {
    Serial.println("Kondisi Pakan : Cukup");
   for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  } else if (pakan > pakan_banyak) {
    Serial.println("Kondisi Pakan : Banyak");
   for (pos = 0; pos <= 0; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 0; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
  } 
}

int loadCell() {
  int GRAM = scale.get_units();
  Serial.println(GRAM);
  return GRAM;
}
