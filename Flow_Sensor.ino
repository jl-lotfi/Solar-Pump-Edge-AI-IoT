#define FLOW_PIN 2 // Set your hardware interrupt pin

volatile uint32_t pulseCount = 0;
float flowRate_Lmin = 0.0;

// Exposing the function to internal RAM keeps interrupt latency under 0.4 µs
void IRAM_ATTR flowISR() {
  pulseCount++; // Atomic increment in RAM
}

void setup() {
  pinMode(FLOW_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), flowISR, RISING);
}

void loop() {
  // Read safely every 1 second
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis >= 1000) {
    lastMillis = millis();
    
    portDISABLE_INTERRUPTS();
    uint32_t count = pulseCount; 
    pulseCount = 0;
    portENABLE_INTERRUPTS();
    
    flowRate_Lmin = (float)count / 7.5f; // K_factor = 7.5 pulses/litre
  }
}
