#include <WiFi.h>
#include <PubSubClient.h>
#include "Config.h"
#include "TempSensorTMP36.h"
#include "Led.h"

TempSensorTMP36 tempSensor(TEMP_SENSOR_PIN);
Led greenLed(GREEN_LED_PIN);
Led redLed(RED_LED_PIN);

const char* SSID = "TP-LINK_2658";
const char* PASSWORD = "66864073";
const char* MQTT_SERVER = "test.mosquitto.org"; //broker.mqtt-dashboard.com";
const char* TOPIC_TEMP_DATA = "temperature/data";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

enum State {
  INIT,
  WIFI_CONNECTING,
  MQTT_CONNECTING,
  SAMPLING,
  PUBLISHING,
  ERROR
};

State currentState = INIT;

unsigned long lastSampleTime = 0;
const unsigned long SAMPLING_INTERVAL = 5000;
unsigned long lastReconnectAttempt = 0;
const unsigned long RECONNECT_INTERVAL = 5000;

void setup() {
  Serial.begin(115200);
  greenLed.off();
  redLed.off();
}

void loop() {
  unsigned long now = millis();
  
  switch (currentState) {
    case INIT:
      handleInitState();
      break;
      
    case WIFI_CONNECTING:
      handleWifiConnectingState();
      break;
      
    case MQTT_CONNECTING:
      handleMqttConnectingState(now);
      break;
      
    case SAMPLING:
      handleSamplingState(now);
      break;
      
    case PUBLISHING:
      handlePublishingState();
      break;
      
    case ERROR:
      handleErrorState(now);
      break;
  }

  updateLeds();
  redLed.update();
  greenLed.update();
  
  if (currentState != INIT && currentState != WIFI_CONNECTING) {
    mqttClient.loop();
  }
}

void handleInitState() {
  Serial.println("Initializing...");
  WiFi.begin(SSID, PASSWORD);
  currentState = WIFI_CONNECTING;
  lastReconnectAttempt = millis();
}

void handleWifiConnectingState() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected! IP address: ");
    Serial.println(WiFi.localIP());
    mqttClient.setServer(MQTT_SERVER, 1883);
    currentState = MQTT_CONNECTING;
  } else {
    Serial.print(".");
    delay(500);
  }
}

void handleMqttConnectingState(unsigned long now) {
  if (now - lastReconnectAttempt > RECONNECT_INTERVAL) {
    lastReconnectAttempt = now;
    
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32-Temp-Monitor")) {
      Serial.println("connected");
      currentState = SAMPLING;
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying later");
      currentState = ERROR;
    }
  }
}

void handleSamplingState(unsigned long now) {
  if (now - lastSampleTime >= SAMPLING_INTERVAL) {
    float temp = tempSensor.getTemperature();
    Serial.print("Temperature: ");
    Serial.println(temp);
    
    if (publishTemperature(temp)) {
      currentState = PUBLISHING;
    } else {
      currentState = ERROR;
    }
    
    lastSampleTime = now;
  }
}

void handlePublishingState() {
  currentState = SAMPLING;
}

void handleErrorState(unsigned long now) {
  if (now % 1000 < 500) {
    redLed.on();
  } else {
    redLed.off();
  }
  
  if (now - lastReconnectAttempt > RECONNECT_INTERVAL) {
    if (WiFi.status() != WL_CONNECTED) {
      currentState = INIT;
    } else if (!mqttClient.connected()) {
      currentState = MQTT_CONNECTING;
    } else {
      currentState = SAMPLING;
    }
  }
}

bool publishTemperature(float temperature) {
  char tempMsg[50];
  snprintf(tempMsg, 50, "%.2f", temperature);
  
  if (mqttClient.publish(TOPIC_TEMP_DATA, tempMsg)) {
    Serial.println("Publish successful");
    return true;
  } else {
    Serial.println("Publish failed");
    return false;
  }
}

void updateLeds() {
  if (currentState == SAMPLING || currentState == PUBLISHING) {
    greenLed.on();
    redLed.off();
  } else if (currentState == ERROR) {
    greenLed.off();
  } else {
    greenLed.off();
    redLed.off();
  }
}
