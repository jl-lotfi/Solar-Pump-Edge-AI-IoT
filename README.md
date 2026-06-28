# Intelligent Solar Pumping: Edge AI & Industrial IoT for Smart Maintenance

## Project Overview
This repository contains our engineering graduation project (PFE). We built an edge-powered predictive maintenance system to protect solar-powered motor pumps from critical failures. By running **TinyML** directly on an **ESP32 microcontroller**, the system detects anomalies locally. This removes the need for a permanent cloud connection, making it perfect for remote agricultural zones where internet is unreliable.

### Project Highlights
* **Fast Local Inference:** 8.7 ms latency running on ESP32 Core 1.
* **High Accuracy:** 96.8% precision in identifying operational faults.
* **Efficient MPPT:** \(\geq\) 99.2% tracking efficiency under stable conditions.
* **Cost Effective:** System pays for itself in roughly 2.8 years compared to running a diesel pump.

---

## Hardware & System Sizing

### 1. Hydraulics & Mechanics
* **Motor Pump Power:** 5.5 HP (4.1 kW output)
* **Target Flow Rate (\(Q\)):** \(12 \text{ m}^3/\text{h}\) (Designed for a \(120 \text{ m}^3/\text{day}\) requirement over 10 hours)
* **Total Dynamic Head (HMT):** \(45\text{ m}\) (\(H_s = 8\text{ m}\) suction, \(H_d = 32\text{ m}\) discharge, \(\Sigma\text{ losses} = 5\text{ m}\))
* **Hydraulic Power (\(P_h\)):** 1.47 kW

### 2. Photovoltaic Sizing
* **Solar Array:** 6.08 kWc total (16 Monocrystalline PERC 380 W panels)
* **Wiring Setup:** 4 Strings in Parallel \(\times\) 4 Panels in Series (4S \(\times\) 4P)
* **DC Bus Voltage:** 380 Vdc
* **Absorbed Power (\(P_e\)):** 4.82 kW (Sizing safety overhead ratio: 1.26)

### 3. Microcontroller & VFD
* **VFD Selection:** Invertek Optidrive P2 (5.5 kW, IP55, CE/UL certified) utilizing a hardware-based P&O MPPT algorithm.
* **Main Controller:** ESP32 Dual-Core Xtensa LX6 @ 240 MHz (520 KB SRAM + 4 MB external PSRAM).

---

## Hardware Pinout & Signal Conditioning

To keep latency low, the ESP32 splits processing tasks across both cores:
* **Core 0:** Handles Wi-Fi connectivity and secure TLS MQTT messaging.
* **Core 1:** Manages direct Modbus RTU polling over RS485 and runs the TinyML model.

### Sensors & Circuit Protection
* **4-20mA Analog Sensors:** Connected through a high-precision \(249\text{ }\Omega\) shunt resistor (0.1% tolerance) to scale the current loop down to a \(1\text{--}5\text{ V}\) range safe for the ESP32 ADC.
* **YF-S201 Flow Sensor:** Tied directly to hardware interrupts (`IRAM_ATTR`), keeping interrupt latency under 0.4 µs.
* **Circuit Protection:** Built-in TVS diodes (\(\pm15\text{ kV}\) ESD), 500mA self-resettable PPTC fuses, and MOV varistors on power lines.

---

## Embedded TinyML Pipeline

We trained a lightweight **3-layer LSTM Network (64 units)** using a dataset of over 10,000 samples. The model classifies four major operating conditions:
1. **Nominal State** (Normal running)
2. **Cavitation** (Pump bubbling/air pockets)
3. **Dry Running** (Marche à Sec)
4. **Electrical Overload** (Surcharge)

### Model Quantization
To fit the model onto the ESP32's limited internal memory, we converted it from floating-point to integer format:
* **Original FP32 Size:** 2.4 MB \(\rightarrow\) **Quantized INT8 Size:** 196 KB
* **Accuracy Loss:** Less than 1.2% after quantization.

### Health Score Logic
The firmware evaluates physical thresholds alongside TinyML classifications to calculate a continuous Health Score:

\[\text{Health Score} = \sum w_i \cdot f_i(x_i)\]

| Monitored Metric | Weight (\(w_i\)) | Normal Limit | Targeted Failure Condition |
| :--- | :--- | :--- | :--- |
| \(I_{\text{motor}} / I_{\text{nominal}}\) | 30% | < 1.15 | Overcurrent / Motor Surcharge |
| \(\Delta Q / Q_{\text{ref}}\) | 25% | > 15% | Flow Rate Drop |
| Motor Temperature | 20% | < 80 °C | Stator Overheating |
| \(\Delta V_{\text{bus}}\) | 15% | < ±5% | Unstable Solar Generation |
| Vibrations (\(g\)) | 10% | < 0.5g | Cavitation / Structural Unbalance |

* **Score > 0.85:** System Healthy.
* **Score 0.60 - 0.85:** Warning state (Triggers diagnostic flags and cloud logging).
* **Score < 0.60:** Critical Fault. The ESP32 immediately sends a shutdown command to Modbus register `0x2000 = 0x0001` to save the motor.

---

## Telemetry Data Format (MQTT Payload)
The system updates a local Mosquitto broker or HiveMQ cloud instance with small, bandwidth-friendly JSON payloads:

```json
{
  "ts": 1717500000,
  "id": "PUMP-01",
  "fw": "2.4.1",
  "freq_hz": 48.72,
  "current_A": 11.34,
  "flow_Lmin": 196.5,
  "temp_C": 68.2,
  "dc_bus_V": 378.5,
  "health": 0.91,
  "mode": "RUN",
  "ai_class": "nominal",
  "rssi_dBm": -62
}
```

---

## Setting Up & Flashing
1. Open the `/firmware` folder using **PlatformIO** in VS Code.
2. Install the required libraries via the manager: `ModbusMaster`, `PubSubClient`, and `ArduinoJson`.
3. Open the main header file to configure your local Wi-Fi and MQTT credentials.
4. Build and flash the project directly to your ESP32 dev board.

---

## Team & Academic Context
* **Students:** Jlassi Lotfi & Najar Soumaya
* **Project Advisors:** Mrs. MOUELHI Sara & Mr. KAMOUN Anis
* **Academic Year:** 2025 – 2026
* **Specialization:** Electrical Engineering – Industrial Informatics & IoT
