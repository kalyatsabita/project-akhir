// Calibrating the load cell
#include "HX711.h"

#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 4
#define calibration_factor 890

HX711 scale;

int GRAM;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
}

void loop() {
  scale.set_scale(calibration_factor);
  GRAM = scale.get_units(), 4;
  Serial.println(GRAM);
  delay (1000);
}
