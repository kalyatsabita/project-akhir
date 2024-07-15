#include "RTClib.h"
#include <ESP32Servo.h>

#define PIN_SG90 13

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
RTC_DS3231 rtc;
int pos = 0;    // variable to store the servo position

int ringhour1 = 10;
int ringmi1 = 22;
int ringhour2 = 10;
int ringmi2 = 24;
int ringhour3 = 10;
int ringmi3 = 26;

void setup() {
  rtc.begin();
  myservo.setPeriodHertz(50); // PWM frequency for SG90
  myservo.attach(PIN_SG90, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180
  Serial.begin(115200);
}

void loop() {
//  delay(1000);
  DateTime now = rtc.now();
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
  
  if ( (ringhour1==now.hour()&& ringmi1==now.minute()) ||(ringhour2==now.hour()&& ringmi2==now.minute()) || (ringhour3==now.hour()&& ringmi3==now.minute())  )
  {
  
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
//   delay(100);
  }
//  delay(1000);
}
