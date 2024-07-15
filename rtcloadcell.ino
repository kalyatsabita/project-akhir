#include <ESP32Servo.h>
#include "HX711.h"

#define PIN_SG90 13
#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 4
#define calibration_factor 837

Servo myservo;
HX711 scale;

int pos = 0;
//int value;
int GRAM;

const int pakan_cukup1 = 30;
const int pakan_cukup2 = 40;
const int pakan_cukup3 = 50;
const int pakan_cukup4 = 60;
const int pakan_banyak1 = 50;
const int pakan_banyak2 = 60;
const int pakan_banyak3 = 70;
const int pakan_banyak4 = 80;

void setup() {
  myservo.setPeriodHertz(50);
  myservo.attach(PIN_SG90, 500, 2400);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
  Serial.begin(115200);
}

void loop() {
  delay(100);
  Serial.println("Feeding time...");
  int value = loadCell();
  Serial.println(value);
  if (value == 0) {
    Serial.println("Kondisi Pakan : Kosong");
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(20);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(20);                       // waits 15ms for the servo to reach the position
    }
  }

 if ((value > 0 && value <= pakan_cukup1) || (value > 0 && value <= pakan_cukup2) || (value > 0 && value <= pakan_cukup3) || (value > 0 && value <= pakan_cukup4)) {
   Serial.println("Kondisi Pakan : Sedikit");
   for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(20);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(20);                       // waits 15ms for the servo to reach the position
    }
 }
 if ((value > pakan_cukup1 && value <= pakan_banyak1) || (value > pakan_cukup2 && value <= pakan_banyak2) || (value > pakan_cukup3 && value <= pakan_banyak3) || (value > pakan_cukup4 && value <= pakan_banyak4)) {
   Serial.println("Kondisi Pakan : Cukup");
   for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(20);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(20);                       // waits 15ms for the servo to reach the position
    }
 }
 if ((value > pakan_banyak1) || (value > pakan_banyak2) || (value > pakan_banyak3) || (value > pakan_banyak4)){
   Serial.println("Kondisi Pakan : Banyak");
   for (pos = 0; pos <= 0; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(20);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 0; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(20);                       // waits 15ms for the servo to reach the position
    }
 }
}

int loadCell() {
  scale.set_scale(calibration_factor);
  int GRAM = scale.get_units();
  Serial.println(GRAM);
  // Your load cell logic to read the value
  return GRAM; // Replace this with actual value from load cell
}
