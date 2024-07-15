#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define ssid "Gaada Wifi"
#define password "kalyaarya2019"
#define mqtt_server "192.168.42.186"
#define mqtt_user ""
#define mqtt_password ""

#define mqtt_topic "servo_minum"

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

#define PIN_SG90 13 // Output pin used

Servo sg90;

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

void callback(char* topic, byte* payload, unsigned int length) {
  String string;

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    // Serial.print ((char)payload[i]);
      string+=((char)payload[i]);
  }

      Serial.print(string);

  if ( topic = "servo_minum")
      Serial.print(" ");
      int status = string.toInt();
      int pos = map(status, 1, 100, 0, 180);  
      Serial.println(pos);
      sg90.write(pos);
      delay(15);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)){
      client.subscribe("servo_minum");
      Serial.println("Connected");  
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
  connect_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  sg90.setPeriodHertz(50); // PWM frequency for SG90
  sg90.attach(PIN_SG90, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
//  client.publish(mqtt_topic, String(pos).c_str());
  delay(1000);
}
