# BLE Jammer - Breadboard Wiring (No Resistors/Capacitors)

## Quick Setup - Using Breadboard & Built-in Pull-ups

This guide shows how to build WITHOUT external resistors or capacitors using a breadboard for testing.

---

## Pin Connection Summary

### **ESP32-S to nRF24 #1 (VSPI)**
```
ESP32 GPIO 15 → nRF24 #1 CE
ESP32 GPIO 5  → nRF24 #1 CSN
ESP32 GPIO 18 → nRF24 #1 SCK
ESP32 GPIO 23 → nRF24 #1 MOSI
ESP32 GPIO 19 → nRF24 #1 MISO
ESP32 3.3V    → nRF24 #1 VCC
ESP32 GND     → nRF24 #1 GND
```

### **ESP32-S to nRF24 #2 (HSPI)**
```
ESP32 GPIO 22 → nRF24 #2 CE
ESP32 GPIO 21 → nRF24 #2 CSN
ESP32 GPIO 14 → nRF24 #2 SCK
ESP32 GPIO 13 → nRF24 #2 MOSI
ESP32 GPIO 12 → nRF24 #2 MISO
ESP32 3.3V    → nRF24 #2 VCC
ESP32 GND     → nRF24 #2 GND
```

### **ESP32-S to OLED Display (I2C)**
```
ESP32 GPIO 27 → OLED SDA
ESP32 GPIO 26 → OLED SCL
ESP32 3.3V    → OLED VCC
ESP32 GND     → OLED GND
```

### **ESP32-S to Buttons (No External Resistors!)**
```
ESP32 GPIO 2  → Button UP → GND
ESP32 GPIO 3  → Button DOWN → GND
ESP32 GPIO 4  → Button SELECT → GND
```

> **Note:** Built-in pull-ups enabled in firmware (`INPUT_PULLUP`)

---

## Breadboard Layout

### **Power Rails**
- **Left rail (+):** Connect to ESP32 3.3V
- **Left rail (-):** Connect to ESP32 GND
- **Right rail (+):** Connect to ESP32 3.3V
- **Right rail (-):** Connect to ESP32 GND

### **nRF24 #1 (Left side of breadboard)**
```
Row 10:  nRF24 VCC  →  Power Rail (+)
Row 11:  nRF24 GND  →  Power Rail (-)
Row 12:  nRF24 CE   →  GPIO 15
Row 13:  nRF24 CSN  →  GPIO 5
Row 14:  nRF24 SCK  →  GPIO 18
Row 15:  nRF24 MOSI →  GPIO 23
Row 16:  nRF24 MISO →  GPIO 19
```

### **nRF24 #2 (Center of breadboard)**
```
Row 20:  nRF24 VCC  →  Power Rail (+)
Row 21:  nRF24 GND  →  Power Rail (-)
Row 22:  nRF24 CE   →  GPIO 22
Row 23:  nRF24 CSN  →  GPIO 21
Row 24:  nRF24 SCK  →  GPIO 14
Row 25:  nRF24 MOSI →  GPIO 13
Row 26:  nRF24 MISO →  GPIO 12
```

### **OLED Display (Right side)**
```
Row 30:  OLED VCC  →  Power Rail (+)
Row 31:  OLED GND  →  Power Rail (-)
Row 32:  OLED SDA  →  GPIO 27
Row 33:  OLED SCL  →  GPIO 26
```

### **Buttons (Bottom rows)**
```
Row 40:  Button UP    →  GPIO 2  →  GND Rail
Row 41:  Button DOWN  →  GPIO 3  →  GND Rail
Row 42:  Button SEL   →  GPIO 4  →  GND Rail
```

---

## Visual ASCII Breadboard

```
     A   B   C   D   E   F   G   H   I   J
1   [+] [-] [+] [-] [+] [-] [+] [-] [+] [-]
2   [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
3   [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
...
10  [VCC][VCC][   ][   ][VCC][VCC][   ][   ][VCC][VCC]
    ↓nRF24#1      ↓nRF24#2              ↓OLED

11  [GND][GND][   ][   ][GND][GND][   ][   ][GND][GND]

12  [CE ][GPIO15][CSN][GPIO5][CE ][GPIO22][   ][   ][SDA][GPIO27]
13  [SCK][GPIO18][MOSI][GPIO23][SCK][GPIO14][MOSI][GPIO13][SCL][GPIO26]
14  [MISO][GPIO19][   ][   ][MISO][GPIO12][   ][   ][   ][   ]
...
40  [BTN_UP][GPIO2][GND][   ][BTN_DN][GPIO3][GND][   ][BTN_SEL][GPIO4]
41  [GND][   ][   ][   ][GND][   ][   ][   ][GND][   ]
```

---

## Connection Steps (In Order)

### **Step 1: Power Rails**
1. Plug ESP32 into breadboard rows on left edge
2. Connect ESP32 3.3V → Power Rail (+)
3. Connect ESP32 GND → Power Rail (-)
4. Verify with multimeter

### **Step 2: nRF24 #1 (VSPI)**
5. Connect nRF24 #1 VCC → Power Rail (+)
6. Connect nRF24 #1 GND → Power Rail (-)
7. Connect each SPI pin (CE, CSN, SCK, MOSI, MISO) → corresponding GPIO

### **Step 3: nRF24 #2 (HSPI)**
8. Repeat Step 2 for nRF24 #2 on different breadboard rows

### **Step 4: OLED Display**
9. Connect OLED VCC → Power Rail (+)
10. Connect OLED GND → Power Rail (-)
11. Connect SDA → GPIO 27
12. Connect SCL → GPIO 26

### **Step 5: Buttons**
13. Connect Button UP → GPIO 2, other end → Power Rail (-)
14. Connect Button DOWN → GPIO 3, other end → Power Rail (-)
15. Connect Button SELECT → GPIO 4, other end → Power Rail (-)

---

## Important Notes

⚠️ **NO RESISTORS NEEDED** - Firmware uses `INPUT_PULLUP`

⚠️ **CAPACITORS OPTIONAL** (for now)
- Modules may work fine without them
- If you get random resets, add 100µF caps across nRF24 VCC/GND
- If RF performance is poor, add 0.1µF caps as well

✅ **USE SHORT WIRES** - Especially for SPI and power lines

✅ **POWER SUPPLY** - Use good USB power adapter (5V/2A minimum)

✅ **CHECK I2C ADDRESS** - OLED defaults to 0x3C
- If display doesn't appear, run I2C scanner

---

## Troubleshooting

**nRF24 not initializing?**
- Check power supply voltage (should be 3.3V)
- Verify all SPI connections
- Try shorter wires

**Display not showing?**
- Check I2C connections (SDA/SCL)
- Verify address with I2C scanner sketch
- Look for blown GPIO pins

**Buttons not working?**
- Test with Serial.println() 
- Verify GPIO pins in firmware match breadboard
- Built-in pull-ups should work

**Random resets?**
- Add 100µF capacitors across nRF24 VCC/GND
- Improve power supply quality
- Use thicker wires for power

---

## Next Steps

1. Upload firmware to ESP32
2. Test each component individually
3. Run in jamming mode for 10 minutes
4. If stable → Ready to solder to PCB!
5. If unstable → Add capacitors and retry

