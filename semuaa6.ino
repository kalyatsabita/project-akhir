//#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "RTClib.h"
#include <ESP32Servo.h>
#include "HX711.h"

// ===================
// Select camera mode
// ===================
//#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#include "camera_pins.h"

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
#define PIN_SG90_PAKAN 13
#define PIN_SG90_MINUM 12

// Load cell pins
#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 4
#define calibration_factor 800

// Feeding Schedule
int pos = 0;
int ringhour1 = 11;
int ringmi1 = 30;
int ringhour2 = 12;
int ringmi2 = 0;
int ringhour3 = 12;
int ringmi3 = 30;

// Pakan Levels
//const int pakan_cukup = 30;
//const int pakan_cukup2 = 40;
//const int pakan_cukup3 = 50;
const int pakan_cukup4 = 60;
//const int pakan_banyak = 50;
//const int pakan_banyak2 = 60;
//const int pakan_banyak3 = 70;
const int pakan_banyak4 = 80;

// Water sensor pins
#define POWER 33
#define SIGNAL 32

// Water Levels
const int air_sedang = 500;
const int air_tinggi = 1000;
int value = 0;
int level = 0;

WiFiClient espClient;
PubSubClient client(espClient);
Servo servoPakan;
Servo servoMinum;
RTC_DS3231 rtc;
HX711 scale;

// Function prototypes
//void startCameraServer();
//void setupLedFlash(int pin);
void connect_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
void printDateTime(DateTime now);
void feedPakan(int pakan);
int loadCell();
int waterSensor();
void moveServo(Servo &servo, int start, int end);

void setup() {
  Serial.begin(115200);
//  Serial.setDebugOutput(true);
  // Setup WiFi
  connect_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

 /* // Setup camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }
  s->set_framesize(s, FRAMESIZE_QVGA);

  #if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
  #endif
  
  startCameraServer();
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
*/
  // Setup servo
  servoPakan.setPeriodHertz(50);
  servoPakan.attach(PIN_SG90_PAKAN, 500, 2400);
  servoMinum.setPeriodHertz(50);
  servoMinum.attach(PIN_SG90_MINUM, 500, 2400);

  pinMode (POWER, OUTPUT);
  digitalWrite(POWER, LOW);
  
  // Setup RTC
  rtc.begin();

  // Setup scale
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
  
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

  int waterLevel = waterSensor();
  Serial.print("Water Level: ");
  Serial.println(waterLevel);
  client.publish(mqtt_topic_minum, String(waterLevel).c_str());

  delay(1000);
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

  if (String(topic) == mqtt_topic_minum) {
    int value = message.toInt();
    int level = waterSensor();
    if (value == 0) {
      Serial.println("Water Level: Empty");
      moveServo(servoMinum, 0, 180);
    } else if (value > 0 && value <= air_sedang) {
      Serial.println("Water Level: Low");
      moveServo(servoMinum, 0, 180);
    } else if (value > air_sedang && value <= air_tinggi) {
      Serial.println("Water Level: Medium");
      moveServo(servoMinum, 0, 90);
    } else if (value > air_tinggi) {
      Serial.println("Water Level: High");
      moveServo(servoMinum, 0, 0);
    }
    client.publish(mqtt_topic_minum, String(level).c_str());
    delay(1000);
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
  Serial.print("Feed time, pakan: ");
  Serial.println(pakan);

  if (pakan == 0) {
    Serial.println("Kondisi Pakan : Kosong");
    moveServo(servoPakan, 0, 180);                       // waits 15ms for the servo to reach the position
  } else if (pakan > 0 && pakan <= pakan_cukup) {
    Serial.println("Kondisi Pakan : Sedikit");
    moveServo(servoPakan, 0, 180);
  } else if (pakan > pakan_cukup && pakan <= pakan_banyak) {
    Serial.println("Kondisi Pakan : Cukup");
    moveServo(servoPakan, 0, 90);
  } else if (pakan > pakan_banyak) {
    Serial.println("Kondisi Pakan : Banyak");
    moveServo(servoPakan, 0, 0);
  } 
}

int loadCell() {
  int GRAM = scale.get_units();
  Serial.println(GRAM);
  return GRAM;
}

int waterSensor() {
  digitalWrite(POWER, HIGH);
  delay(10);
  int value = analogRead(SIGNAL);
  delay(10);
  digitalWrite(POWER, LOW);
  return value;
}

void moveServo(Servo &servo, int start, int end) {
//  digitalWrite(POWER, HIGH);
  delay(10);
  for (int pos = start; pos <= end; pos += 1) {
    servo.write(pos);
    delay(15);
  }
  delay(1000);
  for (int pos = end; pos >= start; pos -= 1) {
    servo.write(pos);
    delay(15);
  }
//  digitalWrite(POWER, LOW);
}
