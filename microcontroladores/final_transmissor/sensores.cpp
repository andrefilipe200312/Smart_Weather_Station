#include "sensores.h"

#define SDA_PIN      21
#define SCL_PIN      22
#define PIN_INT2     4
#define PIN_BATERIA  33
#define PCF8574_ANEMO_ADDR  0x20
#define PCF8574_VENTO_ADDR  0x26

const float raioAnemometro = 0.08;
const float mmPorPulso     = 0.053;

const int reedSwitchPin    = 1;
const int pinPluviometro   = 3;

Adafruit_AHTX0 aht;

volatile int pulsos = 0;
volatile int pulsosPluvio = 0;
bool ultimoEstado = HIGH;
bool ultimoEstadoPluvio = HIGH;
volatile bool pcfInterrupt_2 = false;

int ultimaDirecaoGraus = -1;

float ultimaTemperatura = 0.0;
float ultimaHumidade = 0.0;

void IRAM_ATTR handleInterrupt_2() {
  pcfInterrupt_2 = true;
}

void iniciar_aht10() {
  if (!aht.begin()) {
    Serial.println("Erro ao iniciar AHT10! Reiniciando...");
    delay(1000);
    esp_restart();
  } else {
    Serial.println(" AHT10 iniciado.");
  }
}

void iniciar_sensores() {
  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(PIN_INT2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_INT2), handleInterrupt_2, FALLING);

  Wire.requestFrom(PCF8574_VENTO_ADDR, 1);
  while (Wire.available()) Wire.read();
  pcfInterrupt_2 = false;
}

float calcularPorcentagemBateria() {
  float leitura = analogRead(PIN_BATERIA);
  return map(leitura, 0.0f, 4095.0f, 0, 100);
}

float calcularVelocidadeVento(unsigned long intervalo_ms) {
  float freq = pulsos / (intervalo_ms / 1000.0);
  pulsos = 0;
  float velocidade =  2 * 3.1416 * raioAnemometro * freq;
  return velocidade * 3.6;
}

float calcularPluviosidade() {
  float mm = pulsosPluvio * mmPorPulso;
  pulsosPluvio = 0;
  return mm;
}

void verificarPulso() {
  if (!pcfInterrupt_2) return;
  noInterrupts(); pcfInterrupt_2 = false; interrupts();

  unsigned long timeout = millis();
  Wire.requestFrom(PCF8574_VENTO_ADDR, 1);
  while (Wire.available() == 0) {
    if (millis() - timeout > 1000) {
      Serial.println(" Timeout ao ler PCF8574 (vento). Reiniciando...");
      delay(1000);
      esp_restart();
    }
  }

  byte estadoPCF = Wire.read();
  bool estadoAtualAnemo = bitRead(estadoPCF, reedSwitchPin);
  if (ultimoEstado == HIGH && estadoAtualAnemo == LOW) pulsos++;
  ultimoEstado = estadoAtualAnemo;

  bool estadoAtualPluvio = bitRead(estadoPCF, pinPluviometro);
  if (ultimoEstadoPluvio == HIGH && estadoAtualPluvio == LOW) pulsosPluvio++;
  ultimoEstadoPluvio = estadoAtualPluvio;
}

int lerDirecaoVento() {
  Wire.requestFrom(PCF8574_ANEMO_ADDR, 1);
  unsigned long timeout = millis();
  while (Wire.available() == 0) {
    if (millis() - timeout > 1000) {
      Serial.println(" Timeout ao ler direção do vento. Reiniciando...");
      delay(1000);
      esp_restart();
    }
  }

  byte estado = Wire.read();

  bool reedOeste     = !(estado & (1 << 0));
  bool reedSudoeste  = !(estado & (1 << 1));
  bool reedSul       = !(estado & (1 << 2));
  bool reedSudeste   = !(estado & (1 << 3));
  bool reedEste      = !(estado & (1 << 4));
  bool reedNordeste  = !(estado & (1 << 5));
  bool reedNorte     = !(estado & (1 << 6));
  bool reedNoroeste  = !(estado & (1 << 7));

  int direcaoAtual = -1;

  if (reedOeste && reedSudoeste) direcaoAtual = 225;
  else if (reedSudoeste && reedSul) direcaoAtual = 225;
  else if (reedSul && reedSudeste) direcaoAtual = 135;
  else if (reedSudeste && reedEste) direcaoAtual = 135;
  else if (reedEste && reedNordeste) direcaoAtual = 90;
  else if (reedNordeste && reedNorte) direcaoAtual = 0;
  else if (reedNorte && reedNoroeste) direcaoAtual = 315;
  else if (reedNoroeste && reedOeste) direcaoAtual = 315;
  else if (reedOeste) direcaoAtual = 270;
  else if (reedSudoeste) direcaoAtual = 225;
  else if (reedSul) direcaoAtual = 180;
  else if (reedSudeste) direcaoAtual = 135;
  else if (reedEste) direcaoAtual = 90;
  else if (reedNordeste) direcaoAtual = 45;
  else if (reedNoroeste) direcaoAtual = 315;
  else {
    direcaoAtual = ultimaDirecaoGraus;
    if (direcaoAtual == -1) direcaoAtual = 0;
  }

  ultimaDirecaoGraus = direcaoAtual;
  return direcaoAtual;
}

medidas medir_temp() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  if (isnan(temp.temperature) || temp.temperature == 0 ||
      isnan(humidity.relative_humidity) || humidity.relative_humidity <= 0) {
    Serial.println("Erro ao ler temperatura ou humidade. Reiniciando...");
    delay(1000);
    esp_restart();
  }

  ultimaTemperatura = temp.temperature;
  ultimaHumidade = humidity.relative_humidity;

  return { ultimaTemperatura, ultimaHumidade };
}
