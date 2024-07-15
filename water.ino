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
//  Serial.print("Water Level:");
//  Serial.println(level);
//  delay(1000);
//  value = analogRead(SIGNAL);
  float volume = getWaterVolume(value);
  Serial.print("Nilai Sensor: ");
  Serial.print(level);
  Serial.print(" -> Volume Air: ");
  Serial.print(volume);
  Serial.println(" mililiter");
  delay(1000);
}
//
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

//int sensorPin = A0; // Pin analog yang terhubung ke sensor
//int sensorValue = 0; // Variabel untuk menyimpan nilai sensor

//void setup() {
//  Serial.begin(9600); // Memulai komunikasi serial pada baud rate 9600
//}
//
//void loop() {
//  sensorValue = analogRead(SIGNAL); // Membaca nilai dari sensor
//  Serial.println(sensorValue); // Menampilkan nilai sensor
//  delay(1000); // Penundaan 1 detik sebelum membaca lagi
//}
//
//float getWaterVolume(int sensorValue) {
//  // Gantilah persamaan ini dengan persamaan yang sesuai dari hasil kalibrasi
//  return (sensorValue * faktor_konversi);
//}

//void loop() {
//  sensorValue = analogRead(sensorPin);
//  float volume = getWaterVolume(sensorValue);
//  Serial.print("Nilai Sensor: ");
//  Serial.print(sensorValue);
//  Serial.print(" -> Volume Air: ");
//  Serial.print(volume);
//  Serial.println(" liter");
//  delay(1000);
//}
