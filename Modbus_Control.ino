#include <ModbusMaster.h>

#define DE_RE_PIN 4 // GPIO4 → MAX485 direction
#define MODBUS_BAUD 9600

ModbusMaster node;

void preTransmission() { digitalWrite(DE_RE_PIN, HIGH); }
void postTransmission() { digitalWrite(DE_RE_PIN, LOW); }

void setup() {
  Serial2.begin(MODBUS_BAUD, SERIAL_8N1, 16, 17); // RX=16, TX=17
  node.begin(1, Serial2); // Slave ID = 1 (VFD)
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void loop() {
  uint8_t result = node.readHoldingRegisters(0x2102, 2); // Freq & Current
  if (result == node.ku8MBSuccess) {
    float freq = node.getResponseBuffer(0) * 0.01f; // Hz
    float current = node.getResponseBuffer(1) * 0.10f; // A
  }
  delay(500);
}
