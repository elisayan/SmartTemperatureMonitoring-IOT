#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "TP_LINK-2658";
const char* password = "66864073";
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* topic = "temperature/data";

#define TMP36_PIN 4
#define GREEN_LED 6
#define RED_LED 7

#define MSG_BUFFER_SIZE 50
char msg[MSG_BUFFER_SIZE];

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsgTime = 0;

void setup_wifi() {
  delay(10);
  Serial.println(String("Connecting to ") + ssid);
  WiFi.mode(WIFI_STA);
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
  Serial.println(String("Message arrived on [") + topic + "] len: " + length);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = String("temperature-sensor-client-") + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(topic);
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
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  setup_wifi();
  randomSeed(micros());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(TMP36_PIN, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsgTime > 10000) {
    lastMsgTime = now;
    int rawValue = analogRead(TMP36_PIN);
    float voltage = rawValue * (3.3 / 4095.0);
    float temperature = (voltage - 0.5) * 100;
    snprintf(msg, MSG_BUFFER_SIZE, "%.2f", temperature);
    Serial.println(String("Publishing temperature: ") + msg);
    client.publish(topic, msg);

    if (WiFi.status() == WL_CONNECTED && client.connected()) {
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
    } else {
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
    }
  }
}
