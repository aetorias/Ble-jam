# Dual nRF24 RF Jammer with OLED UI

A custom firmware for educational RF research using dual nRF24L01+PA+LNA modules with ESP32-S, featuring a smooth OLED interface and 4 jamming modes.

## Features

- **Dual nRF24 Modules**: Double the jamming power with parallel RF transmission
- **4 Jamming Modes**:
  - BLE (Bluetooth Low Energy) - Channels 37, 38, 39
  - WiFi Deauth - Channels 1-14
  - Drone Jammer - Random 2.4GHz hopping
  - Bluetooth Classic - Channels 32-80
- **OLED Display**: 128x64 pixel 0.96" I2C display with smooth animations
- **3-Button Control**:
  - UP: Navigate menu
  - DOWN: Navigate menu
  - SELECT: Start/Stop jamming
- **Real-time Statistics**: Packet count, current channel, power level
- **Power Optimization**: Disabled WiFi/BT to maximize RF performance

## Hardware Required

- 1x ESP32-S (38 pin)
- 2x nRF24L01+PA+LNA with PCB board & antenna
- 1x 0.96" OLED I2C display
- 3x Tactile 4-pin push buttons (6x6x6mm)
- 3x 10kΩ resistors (pull-ups for buttons)
- 2x 100µF electrolytic capacitors
- 2x 0.1µF ceramic capacitors
- 2x PCB board 7x9cm
- 22-24 AWG mixed gauge wire
- Soldering iron & solder

## Pinout Configuration

### nRF24 #1 (VSPI)
```
CE   → GPIO 15
CSN  → GPIO 5
SCK  → GPIO 18
MOSI → GPIO 23
MISO → GPIO 19
VCC  → 3.3V
GND  → GND
```

### nRF24 #2 (HSPI)
```
CE   → GPIO 22
CSN  → GPIO 21
SCK  → GPIO 14
MOSI → GPIO 13
MISO → GPIO 12
VCC  → 3.3V
GND  → GND
```

### OLED Display (I2C)
```
SDA  → GPIO 27
SCL  → GPIO 26
VCC  → 3.3V
GND  → GND
```

### Buttons
```
Button UP    → GPIO 2  → GND (with 10kΩ pull-up)
Button DOWN  → GPIO 3  → GND (with 10kΩ pull-up)
Button SEL   → GPIO 4  → GND (with 10kΩ pull-up)
```

### Status LED (Optional)
```
LED → GPIO 25 → GND (through resistor)
```

## Libraries Required

Install via Arduino IDE Library Manager:
- **RF24** by TMRh20
- **U8g2** by Oliver Krause (for OLED display)
- **Wire** (built-in)
- **SPI** (built-in)

## Installation

1. **Install Arduino IDE** if not already installed
2. **Add ESP32 Board**:
   - Go to File → Preferences
   - Add to "Additional Boards Manager URLs": `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Go to Tools → Board Manager
   - Search "ESP32" and install latest version

3. **Install Required Libraries**:
   - Open Sketch → Include Library → Manage Libraries
   - Search and install: `RF24`, `U8g2`

4. **Select Board Settings**:
   - Board: ESP32 Dev Module
   - Upload Speed: 115200
   - CPU Frequency: 80 MHz
   - Flash Size: 4MB
   - Partition Scheme: Default

5. **Connect ESP32** via USB and upload the firmware

## Usage

### Menu Navigation
- Press **UP/DOWN** to scroll through modes
- Press **SELECT** to start jamming on selected mode
- While jamming, press **SELECT** to stop and return to menu

### Display Output

**Main Menu:**
```
RF JAMMER
════════════════════
> IDLE
  BLE
  WiFi
  Drone
  BT Classic
UP/DWN: Nav  SEL: Start
```

**Jamming Screen:**
```
BLE          ON
════════════════════
Jamming |
Packets: 1024
Channel: 37
Power: ████████████████
           [SEL] Stop
```

## Performance

- **BLE Mode**: 2.4GHz channel hopping (37→38→39)
- **WiFi Mode**: 1-14 channel cycling
- **Drone Mode**: Random 2.4GHz spectrum scanning
- **Bluetooth Mode**: 32-80 channel hopping

Both nRF24 modules transmit simultaneously for maximum coverage.

## Troubleshooting

### Display not showing
- Check I2C address (default: 0x3C)
- Verify SDA/SCL connections
- Try I2C scanner sketch to confirm connection

### nRF24 not initializing
- Verify power connections (need good 3.3V supply)
- Check 100µF capacitors are close to module
- Ensure CE/CSN/SPI pins are correct

### Buttons not responding
- Check GPIO pins not in use by other functions
- Verify pull-up resistors (10kΩ)
- Test with Serial.println() to debug

### Serial Monitor Output
Open Serial Monitor at 115200 baud to see initialization messages and debug info.

## Educational Purpose Only

This firmware is for educational research and testing on your own networks only.

**⚠️ WARNING**: Unauthorized use of RF jamming devices is ILLEGAL in most countries. Use responsibly and only in controlled environments with proper permissions.

## License

MIT License - See LICENSE file for details

## Author

Created for educational RF research and exploration.
