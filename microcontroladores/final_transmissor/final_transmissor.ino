#include <SPI.h>
#include <LoRa.h>
#include "sensores.h"

#define SS    5
#define RST   14
#define DIO0  2
#define LED_PIN 27

unsigned long ultimaLeituraVento = 0;
unsigned long ultimaLeituraPluvio = 0;
unsigned long ultimaTransmissao = 0;
unsigned long ultimaVerificacaoReset = 0;

unsigned long intervaloVento  = 60000;
unsigned long intervaloPluvio = 60000;
unsigned long intervaloEnvio  = 120000;
unsigned long intervaloReset  = 86400000UL;  // 24 horas

float velocidadeVento = 0;
float pluviosidade = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  iniciar_aht10();
  iniciar_sensores();

  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Erro ao iniciar LoRa!");
    esp_restart();
  }
  Serial.println("LoRa iniciado.");

  ultimaLeituraVento = millis();
  ultimaLeituraPluvio = millis();
  ultimaTransmissao = millis();
  ultimaVerificacaoReset = millis();
}

void loop() {
  unsigned long agora = millis();

  if (agora - ultimaVerificacaoReset > intervaloReset) {
    Serial.println("Reset diario preventivo.");
    delay(1000);
    esp_restart();
  }

  verificarPulso();

  if (agora - ultimaLeituraVento >= intervaloVento) {
    velocidadeVento = calcularVelocidadeVento(intervaloVento);
    ultimaLeituraVento = agora;
  }

  if (agora - ultimaLeituraPluvio >= intervaloPluvio) {
    pluviosidade = calcularPluviosidade();
    ultimaLeituraPluvio = agora;
  }

  if (agora - ultimaTransmissao >= intervaloEnvio) {
    struct medidas m = medir_temp();
    int direcaoGraus = lerDirecaoVento();
    int percentagem = calcularPorcentagemBateria();

    String mensagem =
      String(m.temperatura, 2) + ";" +
      String(m.humidade, 2) + ";" +
      String(direcaoGraus) + ";" +
      String(velocidadeVento, 2) + ";" +
      String(pluviosidade, 2) + ";" +
      String(percentagem);

    Serial.println("Enviando: " + mensagem);
    digitalWrite(LED_PIN, HIGH);

    if (LoRa.beginPacket() == 0) {
      Serial.println("Erro ao iniciar pacote LoRa. Reiniciando...");
      delay(1000);
      esp_restart();
    }
    LoRa.print(mensagem);
    if (LoRa.endPacket() == 0) {
      Serial.println("Erro ao terminar pacote LoRa. Reiniciando...");
      delay(1000);
      esp_restart();
    }

    digitalWrite(LED_PIN, LOW);
    ultimaTransmissao = agora;
  }
}
