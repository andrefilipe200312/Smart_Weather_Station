#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <esp_system.h>

extern Adafruit_AHTX0 aht;

void iniciar_aht10();
void iniciar_sensores();
struct medidas {
  float temperatura;
  float humidade;
};
medidas medir_temp();
float calcularPorcentagemBateria();
float calcularVelocidadeVento(unsigned long intervalo_ms);
float calcularPluviosidade();
void verificarPulso();
int lerDirecaoVento();
void IRAM_ATTR handleInterrupt_2();

#endif
