#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define MQTT_USER "your_username"
#define MQTT_PASS "your_password"

WiFiClient espClient; 
PubSubClient mqttClient(espClient);

void connectMQTT() {
  while (!mqttClient.connected()) {
    String clientId = "PUMP-" + WiFi.macAddress();
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      mqttClient.subscribe("pompe/01/cmd"); // Listen for incoming commands
    } else { 
      delay(5000); // Retry connection every 5 seconds
    } 
  }
}

void publishTelemetry(float freq, float current, float flow, float health) {
  StaticJsonDocument<256> doc;
  doc["freq_hz"] = freq; 
  doc["current_A"] = current;
  doc["flow_Lmin"] = flow; 
  doc["health"] = health;
  
  char buf[256]; 
  serializeJson(doc, buf);
  mqttClient.publish("pompe/01/telemetry", buf, false); // Send payload at QoS 0
}

void setup() {
  // Initialize your network configuration here
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();
}
