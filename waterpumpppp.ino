#include <WiFi.h>
#include <PubSubClient.h>

#define ssid "Hai"
#define password "20012003"
#define mqtt_server "192.168.43.7"
#define mqtt_user ""
#define mqtt_password ""

#define mqtt_topic "minum"

WiFiClient espClient;
PubSubClient client(espClient);

#define RELAY_PIN 18  // Pin untuk relay yang mengontrol pompa
#define POWER_PIN 33  // Pin untuk memberi daya ke sensor air
#define SIGNAL_PIN 32 // Pin untuk membaca nilai dari sensor air

const int air_sedang = 500;
const int air_tinggi = 1000;
int sensorValue = 0;

void connect_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
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

void controlPump(bool state) {
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

int waterSensor() {
  digitalWrite(POWER_PIN, HIGH);
  delay(10);
  int value = analogRead(SIGNAL_PIN);
  digitalWrite(POWER_PIN, LOW);
  return value;
}

void setup() {
  Serial.begin(115200);
  connect_wifi();
  client.setCallback(callback);
  client.setServer(mqtt_server, 1883);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sensorValue = waterSensor();
  Serial.print("Water Level: ");
  Serial.println(sensorValue);

  if (sensorValue == 0) {
    Serial.println("Water Level: Empty");
    controlPump(true);
  } else if (sensorValue > 0 && sensorValue <= air_sedang) {
    Serial.println("Water Level: Low");
    controlPump(true);
  } else if (sensorValue > air_sedang && sensorValue <= air_tinggi) {
    Serial.println("Water Level: Medium");
    controlPump(true);
  } else if (sensorValue > air_tinggi) {
    Serial.println("Water Level: High");
    controlPump(false);
  }

  client.publish(mqtt_topic, String(sensorValue).c_str());
  delay(1000); // Delay between sensor readings
}
