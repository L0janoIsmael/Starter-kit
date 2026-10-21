// ============================================================================
// ESP32 - Flechas OLED + LEDs (reles) en bucle automatico
// Archivo unico
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================== CONFIGURACION ===============================
#define I2C_SDA_PIN        21
#define I2C_SCL_PIN        22
#define I2C_FREQUENCY_HZ   400000

#define OLED_I2C_ADDRESS   0x3C
#define OLED_WIDTH         128
#define OLED_HEIGHT        64
#define OLED_RESET_PIN     -1

#define LOGO_TIME_MS       3000   // ventana de arranque (logo)
#define LOOP_STEP_MS       1500   // tiempo de cada flecha

// Rele -> LED (segun el diagrama)
#define PIN_VERDE          18   // relay2 -> LED verde
#define PIN_AMARILLO       19   // relay1 -> LED amarillo
#define PIN_AZUL           4    // relay3 -> LED azul
#define PIN_BLANCO         23   // relay4 -> LED blanco

// Nivel que ACTIVA el rele. Si los LEDs funcionan al reves, cambia a LOW.
#define RELAY_ON           HIGH
#define RELAY_OFF          (!RELAY_ON)

// ============================== INSTANCIAS ==================================
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET_PIN);

bool bootComplete = false;
unsigned long bootTime = 0;

// Secuencia del bucle: arriba, derecha, abajo, izquierda
const char SEQUENCE[] = { 'u', 'r', 'd', 'l' };
const uint8_t SEQ_LEN = sizeof(SEQUENCE);
uint8_t seqIndex = 0;
unsigned long lastChange = 0;

// ================================= I2C ======================================
void initI2C() {
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(I2C_FREQUENCY_HZ);
  Serial.print("[I2C] bus listo SDA=");
  Serial.print(I2C_SDA_PIN);
  Serial.print(" SCL=");
  Serial.println(I2C_SCL_PIN);
}

void scanI2C() {
  int count = 0;
  for (int address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("[I2C] dispositivo en 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      count++;
    }
  }
  Serial.print("[I2C] dispositivos encontrados: ");
  Serial.println(count);
}

void testI2CDevice() {
  Wire.beginTransmission(OLED_I2C_ADDRESS);
  if (Wire.endTransmission() == 0) {
    Serial.println("[POST] OLED responde en 0x3C");
  } else {
    Serial.println("[POST] OLED no responde. Arranque detenido");
    while (true) { delay(1000); }
  }
}

// ================================ DISPLAY ===================================
void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    Serial.println("[DISPLAY] Error al inicializar SSD1306. Arranque detenido");
    while (true) { delay(1000); }
  }
  Serial.println("[DISPLAY] SSD1306 listo 128x64 a 400 kHz");
}

void showLogo() {
  display.clearDisplay();
  display.drawRect(0, 0, OLED_WIDTH, OLED_HEIGHT, SSD1306_WHITE);
  display.display();
}

void testDisplay() {
  int lado = 8;
  int x = (OLED_WIDTH - lado) / 2;
  int y = (OLED_HEIGHT - lado) / 2;
  display.fillRect(x, y, lado, lado, SSD1306_WHITE);
  display.display();
  Serial.print("[POST] cuadrado x=");
  Serial.print(x);
  Serial.print(" y=");
  Serial.print(y);
  Serial.print(" lado=");
  Serial.println(lado);
}

// ============================ LEDs (RELES) Y FLECHAS ========================
void initLeds() {
  pinMode(PIN_VERDE, OUTPUT);
  pinMode(PIN_AMARILLO, OUTPUT);
  pinMode(PIN_AZUL, OUTPUT);
  pinMode(PIN_BLANCO, OUTPUT);
  digitalWrite(PIN_VERDE, RELAY_OFF);
  digitalWrite(PIN_AMARILLO, RELAY_OFF);
  digitalWrite(PIN_AZUL, RELAY_OFF);
  digitalWrite(PIN_BLANCO, RELAY_OFF);
  Serial.println("[LEDS] reles listos (todos apagados)");
}

void setLeds(bool verde, bool amarillo, bool azul, bool blanco) {
  digitalWrite(PIN_VERDE,    verde    ? RELAY_ON : RELAY_OFF);
  digitalWrite(PIN_AMARILLO, amarillo ? RELAY_ON : RELAY_OFF);
  digitalWrite(PIN_AZUL,     azul     ? RELAY_ON : RELAY_OFF);
  digitalWrite(PIN_BLANCO,   blanco   ? RELAY_ON : RELAY_OFF);
}

void drawArrow(char dir) {
  display.clearDisplay();
  switch (dir) {
    case 'u':  // arriba
      display.fillTriangle(64, 4, 34, 34, 94, 34, SSD1306_WHITE);
      display.fillRect(54, 34, 20, 26, SSD1306_WHITE);
      break;
    case 'd':  // abajo
      display.fillTriangle(64, 60, 34, 30, 94, 30, SSD1306_WHITE);
      display.fillRect(54, 4, 20, 26, SSD1306_WHITE);
      break;
    case 'r':  // derecha
      display.fillTriangle(124, 32, 88, 4, 88, 60, SSD1306_WHITE);
      display.fillRect(6, 22, 82, 20, SSD1306_WHITE);
      break;
    case 'l':  // izquierda
      display.fillTriangle(4, 32, 40, 4, 40, 60, SSD1306_WHITE);
      display.fillRect(40, 22, 82, 20, SSD1306_WHITE);
      break;
  }
  display.display();
}

// Muestra la flecha y enciende su par de LEDs
void showArrow(char dir) {
  switch (dir) {
    case 'u': setLeds(true,  true,  false, false); Serial.println("[FLECHA] arriba: verde + amarillo");     break;
    case 'd': setLeds(false, false, true,  true ); Serial.println("[FLECHA] abajo: azul + blanco");          break;
    case 'r': setLeds(true,  false, true,  false); Serial.println("[FLECHA] derecha: amarillo + blanco");        break;
    case 'l': setLeds(false, true,  false, true ); Serial.println("[FLECHA] izquierda: verde + azul"); break;
  }
  drawArrow(dir);
  lastChange = millis();
}

// ================================ SETUP / LOOP ==============================
void setup() {
  Serial.begin(115200);
  Serial.println("[BOOT] sistema de flechas OLED");

  initLeds();

  initI2C();
  scanI2C();
  testI2CDevice();

  initDisplay();
  showLogo();
  testDisplay();

  bootTime = millis();
}

void loop() {
  // Estado BOOT: logo en pantalla sin bloquear
  if (!bootComplete) {
    if (millis() - bootTime >= LOGO_TIME_MS) {
      bootComplete = true;
      Serial.println("[FSM] BOOT -> RUN");
      seqIndex = 0;
      showArrow(SEQUENCE[seqIndex]);
    }
    return;
  }

  // Estado RUN: bucle automatico sin delay()
  if (millis() - lastChange >= LOOP_STEP_MS) {
    seqIndex = (seqIndex + 1) % SEQ_LEN;
    showArrow(SEQUENCE[seqIndex]);
  }
}