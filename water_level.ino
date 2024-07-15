#include <WiFi.h>
#include <PubSubClient.h>

#define ssid "Hai"
#define password "20012003"
#define mqtt_server "192.168.43.7"
#define mqtt_user ""
#define mqtt_password ""

#define mqtt_topic "water_level"

WiFiClient espClient;
PubSubClient client(espClient);

#define POWER  33 // ESP32 pin GPIO17 connected to sensor's VCC pin
#define SIGNAL 32 // ESP32 pin GPIO36 (ADC0) connected to sensor's signal pin

int value = 0; // variable to store the sensor value
int level = 0;

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
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(POWER, OUTPUT);   // configure pin as an OUTPUT
  digitalWrite(POWER, LOW); // turn the sensor OFF
  connect_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  f (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client", mqtt_username, mqtt_password);
  level = waterSensor();
  Serial.print("Water Level: ");
  Serial.println(level);
  client.publish(mqtt_topic, String(level).c_str());
  delay(1000);
}

int waterSensor() {
  digitalWrite(POWER, HIGH);
  delay(10);
  value = analogRead(SIGNAL);
  delay(10);
  digitalWrite(POWER, LOW);
  return value;
}
