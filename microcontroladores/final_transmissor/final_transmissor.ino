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

unsigned long intervaloVento  = 60000;
unsigned long intervaloPluvio = 60000;
unsigned long intervaloEnvio  = 120000;

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
    ESP.restart();
  }
  Serial.println("LoRa iniciado.");

  ultimaLeituraVento = millis();
  ultimaLeituraPluvio = millis();
  ultimaTransmissao = millis();
}

void loop() {
  unsigned long agora = millis();
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
    LoRa.beginPacket();
    LoRa.print(mensagem);
    LoRa.endPacket();
    digitalWrite(LED_PIN, LOW);

    ultimaTransmissao = agora;
  }
}
