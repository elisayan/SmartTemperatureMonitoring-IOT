#include <WiFi.h>
#include <PubSubClient.h>
#include "Config.h"
#include "TempSensorTMP36.h"
#include "Led.h"
#include "FSM.h"

TempSensorTMP36 tempSensor(TEMP_SENSOR_PIN);
Led greenLed(GREEN_LED_PIN);
Led redLed(RED_LED_PIN);
FSM fsm;

const char* SSID = "TP-LINK_2658";
const char* PASSWORD = "66864073";
const char* MQTT_SERVER = "test.mosquitto.org";
const char* TOPIC = "temperature/data";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

SystemState currentState = NORMAL;
unsigned long lastSampleTime = 0;
unsigned long tooHotStartTime = 0;

void setup() {
  Serial.begin(115200);
  setupWiFi();
  mqttClient.setServer(MQTT_SERVER, 1883);
}

void loop() {
  unsigned long now = millis();
  
  if(now - lastSampleTime >= fsm.getSamplingInterval()) {
    float temp = tempSensor.getTemperature();
    currentState = fsm.getState(temp);
    
    if(currentState == TOO_HOT && fsm.checkAlarmCondition(tooHotStartTime)) {
      currentState = ALARM;
      handleAlarm();
    }
    
    sendMQTT(temp);
    lastSampleTime = now;
  }

  updateLeds();
  redLed.update();
  mqttClient.loop();
}

void setupWiFi() {
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while(!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if(mqttClient.connect("ESP32-Temp-Monitor")) {
      Serial.println("connected");
      mqttClient.subscribe("control/commands");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(100);
    }
  }
}

void sendMQTT(float temp) {
  if(!mqttClient.connected()) {
    reconnectMQTT();
  }
  
  char tempMsg[50];
  snprintf(tempMsg, 50, "%.2f", temp);
  mqttClient.publish(TOPIC, tempMsg);

  if(currentState == ALARM) {
    mqttClient.publish("system/status", "ALARM_ACTIVE");
  }

  Serial.print("[MQTT] Invio temperatura: ");
  Serial.println(temp);
  
  if(currentState == ALARM) {
    Serial.println("[MQTT] Allarme attivato!");
  }
}

void handleAlarm() {
  redLed.blink(500);
  greenLed.off();
  
  mqttClient.publish("system/status", "ALARM_TRIGGERED");
}

void updateLeds() {
  if(currentState == ALARM) return;
  
  if(WiFi.status() == WL_CONNECTED && mqttClient.connected()) {
    greenLed.on();
    redLed.off();
  } else {
    greenLed.off();
    redLed.on();
  }
}