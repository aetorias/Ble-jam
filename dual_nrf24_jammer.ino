#include <RF24.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <EEPROM.h>

// ==================== PIN CONFIGURATION ====================
// nRF24 #1 (VSPI - Default SPI Bus)
#define NRF24_1_CE    15
#define NRF24_1_CSN   5
#define NRF24_1_MOSI  23  // VSPI
#define NRF24_1_MISO  19  // VSPI
#define NRF24_1_SCK   18  // VSPI

// nRF24 #2 (HSPI - Secondary SPI Bus)
#define NRF24_2_CE    22
#define NRF24_2_CSN   21
#define NRF24_2_MOSI  13  // HSPI
#define NRF24_2_MISO  12  // HSPI
#define NRF24_2_SCK   14  // HSPI

// OLED Display (I2C)
#define OLED_SDA      27
#define OLED_SCL      26

// Buttons (GPIO)
#define BUTTON_UP     2    // Navigate up
#define BUTTON_DOWN   3    // Navigate down
#define BUTTON_SELECT 4    // Start/Stop jamming

// Status LED (optional)
#define STATUS_LED    25

// ==================== OLED SETUP ====================
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA, U8X8_PIN_NONE);

// ==================== RF24 INSTANCES ====================
SPIClass* spiVSPI = nullptr;
SPIClass* spiHSPI = nullptr;
RF24 radioVSPI(NRF24_1_CE, NRF24_1_CSN);
RF24 radioHSPI(NRF24_2_CE, NRF24_2_CSN);

// ==================== JAMMING MODES ====================
enum JamMode {
  MODE_IDLE = 0,
  MODE_BLE = 1,
  MODE_WIFI = 2,
  MODE_DRONE = 3,
  MODE_BT_CLASSIC = 4
};

const char* modeNames[] = {"IDLE", "BLE", "WiFi", "Drone", "BT Classic"};
const char* modeChannels[] = {"OFF", "37-39", "1-14", "2.4GHz", "32-80"};

// ==================== CHANNEL ARRAYS ====================
int ble_channels[] = {2, 26, 80};  // BLE advertising channels
int wifi_channels[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};  // WiFi channels
int bt_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};

// ==================== GLOBAL VARIABLES ====================
JamMode currentMode = MODE_IDLE;
JamMode selectedMode = MODE_IDLE;
bool isJamming = false;

unsigned long packetCount = 0;
unsigned long lastDisplayUpdate = 0;
int currentChannel = 0;
int currentChannelIndex = 0;

// Button debounce
volatile unsigned long lastButtonPress[3] = {0, 0, 0};
const unsigned long DEBOUNCE_DELAY = 50;  // 50ms debounce

// Animation variables
int animationFrame = 0;
unsigned long lastAnimationFrame = 0;
const unsigned long ANIMATION_SPEED = 200;  // 200ms per frame

// ==================== FUNCTION DECLARATIONS ====================
void setupRadios();
void configureRadio(RF24 &radio, SPIClass *spi, int ce, int csn);
void handleButtonPress(int buttonIndex);
void updateDisplay();
void drawMainMenu();
void drawJammingScreen();
void jamBLE();
void jamWiFi();
void jamDrone();
void jamBTClassic();
void executeJam();
void startJam();
void stopJam();
void setupInterrupts();

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Pin configuration
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);
  
  // Initialize OLED
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 30, "Initializing...");
  u8g2.sendBuffer();
  
  Serial.println("\n\n=== DUAL nRF24 JAMMER ===");
  Serial.println("Initializing RF modules...");
  
  // Disable WiFi and Bluetooth to avoid interference
  esp_wifi_stop();
  esp_wifi_deinit();
  esp_bt_controller_deinit();
  
  // Initialize SPI buses
  setupRadios();
  
  // Setup button interrupts
  setupInterrupts();
  
  Serial.println("✓ System Ready!");
  delay(500);
}

// ==================== SETUP RADIOS ====================
void setupRadios() {
  // Initialize VSPI (SPI Bus 1)
  spiVSPI = new SPIClass(VSPI);
  spiVSPI->begin(NRF24_1_SCK, NRF24_1_MISO, NRF24_1_MOSI);
  
  Serial.println("Configuring nRF24 #1 (VSPI)...");
  if (radioVSPI.begin(spiVSPI)) {
    radioVSPI.setAutoAck(false);
    radioVSPI.stopListening();
    radioVSPI.setRetries(0, 0);
    radioVSPI.setPALevel(RF24_PA_MAX);
    radioVSPI.setDataRate(RF24_2MBPS);
    radioVSPI.setCRCLength(RF24_CRC_DISABLED);
    Serial.println("✓ nRF24 #1 initialized!");
  } else {
    Serial.println("✗ nRF24 #1 initialization failed!");
  }
  
  // Initialize HSPI (SPI Bus 2)
  spiHSPI = new SPIClass(HSPI);
  spiHSPI->begin(NRF24_2_SCK, NRF24_2_MISO, NRF24_2_MOSI);
  
  Serial.println("Configuring nRF24 #2 (HSPI)...");
  if (radioHSPI.begin(spiHSPI)) {
    radioHSPI.setAutoAck(false);
    radioHSPI.stopListening();
    radioHSPI.setRetries(0, 0);
    radioHSPI.setPALevel(RF24_PA_MAX);
    radioHSPI.setDataRate(RF24_2MBPS);
    radioHSPI.setCRCLength(RF24_CRC_DISABLED);
    Serial.println("✓ nRF24 #2 initialized!");
  } else {
    Serial.println("✗ nRF24 #2 initialization failed!");
  }
}

// ==================== BUTTON INTERRUPT SETUP ====================
void setupInterrupts() {
  attachInterrupt(digitalPinToInterrupt(BUTTON_UP), []() {
    handleButtonPress(0);
  }, FALLING);
  
  attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN), []() {
    handleButtonPress(1);
  }, FALLING);
  
  attachInterrupt(digitalPinToInterrupt(BUTTON_SELECT), []() {
    handleButtonPress(2);
  }, FALLING);
}

// ==================== BUTTON HANDLER ====================
void handleButtonPress(int buttonIndex) {
  unsigned long now = millis();
  
  // Debounce check
  if (now - lastButtonPress[buttonIndex] < DEBOUNCE_DELAY) {
    return;
  }
  lastButtonPress[buttonIndex] = now;
  
  if (buttonIndex == 0) {
    // UP button - navigate up
    Serial.println("Button: UP pressed");
    if (!isJamming) {
      selectedMode = (JamMode)((selectedMode - 1 + 5) % 5);
    }
  } else if (buttonIndex == 1) {
    // DOWN button - navigate down
    Serial.println("Button: DOWN pressed");
    if (!isJamming) {
      selectedMode = (JamMode)((selectedMode + 1) % 5);
    }
  } else if (buttonIndex == 2) {
    // SELECT button - toggle jamming
    Serial.println("Button: SELECT pressed");
    if (!isJamming) {
      startJam();
    } else {
      stopJam();
    }
  }
}

// ==================== START JAMMING ====================
void startJam() {
  if (selectedMode == MODE_IDLE) {
    return;  // Can't jam in IDLE mode
  }
  
  isJamming = true;
  currentMode = selectedMode;
  packetCount = 0;
  currentChannelIndex = 0;
  
  digitalWrite(STATUS_LED, HIGH);  // Turn on LED
  Serial.print("Starting jam mode: ");
  Serial.println(modeNames[currentMode]);
}

// ==================== STOP JAMMING ====================
void stopJam() {
  isJamming = false;
  currentMode = MODE_IDLE;
  packetCount = 0;
  
  digitalWrite(STATUS_LED, LOW);  // Turn off LED
  Serial.println("Jamming stopped");
}

// ==================== JAM FUNCTIONS ====================
void jamBLE() {
  int channelIndex = packetCount % 3;  // Cycle through 3 BLE channels
  int channel = ble_channels[channelIndex];
  
  radioVSPI.setChannel(channel);
  radioHSPI.setChannel(channel);
  
  // Send junk data
  uint8_t junk[32];
  for (int i = 0; i < 32; i++) {
    junk[i] = random(0, 256);
  }
  
  radioVSPI.write(&junk, 32);
  radioHSPI.write(&junk, 32);
  
  packetCount++;
  currentChannel = channel;
  currentChannelIndex = channelIndex;
}

void jamWiFi() {
  int channelIndex = (packetCount / 10) % sizeof(wifi_channels);
  int channel = wifi_channels[channelIndex];
  
  radioVSPI.setChannel(channel);
  radioHSPI.setChannel(channel);
  
  // Send noise packets
  uint8_t noise[32];
  for (int i = 0; i < 32; i++) {
    noise[i] = random(0, 256);
  }
  
  radioVSPI.write(&noise, 32);
  radioHSPI.write(&noise, 32);
  
  packetCount++;
  currentChannel = channel;
  currentChannelIndex = channelIndex;
}

void jamDrone() {
  // Random hopping across 2.4GHz spectrum
  int channel = random(0, 126);
  
  radioVSPI.setChannel(channel);
  radioHSPI.setChannel(channel);
  
  uint8_t payload[32];
  for (int i = 0; i < 32; i++) {
    payload[i] = random(0, 256);
  }
  
  radioVSPI.write(&payload, 32);
  radioHSPI.write(&payload, 32);
  
  packetCount++;
  currentChannel = channel;
  currentChannelIndex = random(0, 126);
}

void jamBTClassic() {
  int channelIndex = (packetCount / 5) % sizeof(bt_channels);
  int channel = bt_channels[channelIndex];
  
  radioVSPI.setChannel(channel);
  radioHSPI.setChannel(channel);
  
  uint8_t btPayload[32];
  for (int i = 0; i < 32; i++) {
    btPayload[i] = random(0, 256);
  }
  
  radioVSPI.write(&btPayload, 32);
  radioHSPI.write(&btPayload, 32);
  
  packetCount++;
  currentChannel = channel;
  currentChannelIndex = channelIndex;
}

// ==================== EXECUTE JAM ====================
void executeJam() {
  if (!isJamming) return;
  
  switch (currentMode) {
    case MODE_BLE:
      jamBLE();
      break;
    case MODE_WIFI:
      jamWiFi();
      break;
    case MODE_DRONE:
      jamDrone();
      break;
    case MODE_BT_CLASSIC:
      jamBTClassic();
      break;
    default:
      break;
  }
}

// ==================== DISPLAY FUNCTIONS ====================
void updateDisplay() {
  unsigned long now = millis();
  
  if (now - lastDisplayUpdate < 100) {  // Update every 100ms
    return;
  }
  lastDisplayUpdate = now;
  
  u8g2.clearBuffer();
  
  if (isJamming) {
    drawJammingScreen();
  } else {
    drawMainMenu();
  }
  
  u8g2.sendBuffer();
}

// ==================== MAIN MENU SCREEN ====================
void drawMainMenu() {
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(20, 12, "RF JAMMER");
  
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawLine(0, 15, 128, 15);  // Separator line
  
  // Draw menu items
  u8g2.setFont(u8g2_font_6x10_tf);
  for (int i = 0; i < 5; i++) {
    int y = 25 + (i * 8);
    
    if (i == selectedMode) {
      // Highlight selected item
      u8g2.drawStr(5, y, "> ");
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, y - 6, 128, 8);  // Highlight background
      u8g2.setDrawColor(0);
      u8g2.drawStr(15, y, modeNames[i]);
      u8g2.setDrawColor(1);
    } else {
      u8g2.drawStr(15, y, modeNames[i]);
    }
  }
  
  // Bottom info bar
  u8g2.drawLine(0, 56, 128, 56);
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(5, 64, "UP/DWN: Nav  SEL: Start");
}

// ==================== JAMMING SCREEN ====================
void drawJammingScreen() {
  // Header
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(10, 12, modeNames[currentMode]);
  u8g2.drawStr(100, 12, "ON");
  
  // Separator
  u8g2.drawLine(0, 15, 128, 15);
  
  // Animation frame
  animationFrame = (millis() / 200) % 4;
  const char* spinner[] = {"|", "/", "-", "\\"};
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(5, 28, "Jamming ");
  u8g2.drawStr(55, 28, spinner[animationFrame]);
  
  // Stats line 1
  char buf1[30];
  snprintf(buf1, sizeof(buf1), "Packets: %lu", packetCount);
  u8g2.drawStr(5, 40, buf1);
  
  // Stats line 2
  char buf2[30];
  snprintf(buf2, sizeof(buf2), "Channel: %d", currentChannel);
  u8g2.drawStr(5, 50, buf2);
  
  // Power bar
  u8g2.drawStr(5, 62, "Power: ");
  u8g2.drawFrame(45, 55, 75, 8);  // Power bar frame
  u8g2.drawBox(47, 57, 71, 4);    // Full power indicator
  
  // Stop info
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(110, 64, "[SEL] Stop");
}

// ==================== MAIN LOOP ====================
void loop() {
  // Execute jamming
  executeJam();
  
  // Update display
  updateDisplay();
  
  // Small delay to prevent overwhelming the CPU
  delayMicroseconds(100);
}
