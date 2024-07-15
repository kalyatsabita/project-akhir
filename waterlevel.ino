#define POWER 33
#define SIGNAL 32

int value=0;
float faktor_konversi = 0.3;
int level=0;

void setup() {
  Serial.begin(115200);
  pinMode(POWER,OUTPUT);
  digitalWrite(POWER,LOW);
}

void loop() {
  level=waterSensor();
  float volume = getWaterVolume(value);
  Serial.print("Nilai Sensor: ");
  Serial.print(level);
  Serial.print(" -> Volume Air: ");
  Serial.print(volume);
  Serial.println(" mililiter");
  delay(1000);
}

int waterSensor()
{
  digitalWrite(POWER,HIGH);
  delay(10);
  value=analogRead(SIGNAL);
  delay(10);
  digitalWrite(POWER,LOW);
  return value;
}

float getWaterVolume(int value) {
  // Gantilah persamaan ini dengan persamaan yang sesuai dari hasil kalibrasi
  return (value * faktor_konversi);
}
