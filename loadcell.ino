// Calibrating the load cell
#include "HX711.h"

// HX711 circuit wiring
#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 4
#define calibration_factor 890
//const int LOADCELL_DOUT_PIN = 16;
//const int LOADCELL_SCK_PIN = 4;

HX711 scale;
//float calibration_factor = -400; //Nilai awal perkiraan
//float units;
//float ounces;

int GRAM;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
//  scale.set_scale();
// scale.tare();

// long zero_factor = scale.read_average(); 
// Serial.print("Zero factor: ");
// Serial.println(zero_factor);
}
//float calibration_factor = -400; //Nilai awal perkiraan
//float units;
//float ounces;


void loop() {
//  scale.set_scale(calibration_factor);
//
// Serial.print("Pembacaan : ");
// units = scale.get_units(), 10;
// if (units < 0)
// {
// units = 0.00;
// }
// ounces = units * 0.035274;
// Serial.print(units);
// Serial.print(" grams"); 
// Serial.print(" calibration_factor: ");
// Serial.print(calibration_factor);
// Serial.println();
//
// if(Serial.available())
// {
// char temp = Serial.read();
// if(temp == '+' || temp == 'a')
// calibration_factor += 1;
// else if(temp == '-' || temp == 'z')
// calibration_factor -= 1;
// }
//}
//  if (scale.is_ready()) {
//    scale.set_scale();    
//    Serial.println("Tare... remove any weights from the scale.");
//    delay(5000);
//    scale.tare();
//    Serial.println("Tare done...");
//    Serial.print("Place a known weight on the scale...");
//    delay(5000);
//    long reading = scale.get_units(10);
//    Serial.print("Result: ");
//    Serial.println(reading);
//  } 
//  else {
//    Serial.println("HX711 not found.");
//  }
//  delay(1000);
//  if (scale.is_ready()) {
//    long reading = scale.read();
//    Serial.print("HX711 reading: ");
//    Serial.println(reading);
//  } else {
//    Serial.println("HX711 not found.");
//  }

//  delay(1000);
  scale.set_scale(calibration_factor);
  GRAM = scale.get_units(), 4;
  Serial.println(GRAM);
  delay (1000);
}

//calibration factor will be the (reading)/(known weight)
