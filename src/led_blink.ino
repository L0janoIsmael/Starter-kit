// Control de 4 LED mediante módulos de relé y un ESP32 DevKit V4
// Pines según las conexiones del diagrama JSON de Wokwi:
// Verde    -> relay2 -> GPIO 18
// Amarillo -> relay1 -> GPIO 19
// Azul     -> relay3 -> GPIO 21
// Blanco   -> relay4 -> GPIO 22
// En Wokwi, los módulos de relé predeterminados se activan con HIGH.

const int RELE_VERDE = 18;
const int RELE_AMARILLO = 19;
const int RELE_AZUL = 21;
const int RELE_BLANCO = 22;

// Enciende o apaga cada LED a través de su relé.
// HIGH = relé activado; LOW = relé desactivado.
void configurarLeds(bool verde, bool amarillo, bool azul, bool blanco) {
  digitalWrite(RELE_VERDE, verde ? HIGH : LOW);
  digitalWrite(RELE_AMARILLO, amarillo ? HIGH : LOW);
  digitalWrite(RELE_AZUL, azul ? HIGH : LOW);
  digitalWrite(RELE_BLANCO, blanco ? HIGH : LOW);
}

void setup() {
  pinMode(RELE_VERDE, OUTPUT);
  pinMode(RELE_AMARILLO, OUTPUT);
  pinMode(RELE_AZUL, OUTPUT);
  pinMode(RELE_BLANCO, OUTPUT);

  // Comenzar con todos los LED apagados.
  configurarLeds(false, false, false, false);
}

void loop() {
  // 1. Encender y apagar el LED verde.
  configurarLeds(true, false, false, false);
  delay(1000);
  configurarLeds(false, false, false, false);
  delay(500);

  // 2. Encender y apagar el LED amarillo.
  configurarLeds(false, true, false, false);
  delay(1000);
  configurarLeds(false, false, false, false);
  delay(500);

  // 3. Encender y apagar el LED azul.
  configurarLeds(false, false, true, false);
  delay(1000);
  configurarLeds(false, false, false, false);
  delay(500);

  // 4. Encender y apagar el LED blanco.
  configurarLeds(false, false, false, true);
  delay(1000);
  configurarLeds(false, false, false, false);
  delay(500);

  // 5. Encender juntos el LED verde y el amarillo.
  configurarLeds(true, true, false, false);
  delay(1500);

  // 6. Encender los cuatro LED y luego apagarlos todos.
  configurarLeds(true, true, true, true);
  delay(1500);
  configurarLeds(false, false, false, false);
  delay(1000);

  // 7. Encender juntos el LED verde y el azul.
  configurarLeds(true, false, true, false);
  delay(1500);

  // 8. Apagar verde y azul; encender amarillo y blanco.
  configurarLeds(false, true, false, true);
  delay(1500);

  // Apagar todo antes de repetir la secuencia.
  configurarLeds(false, false, false, false);
  delay(1000);
}

