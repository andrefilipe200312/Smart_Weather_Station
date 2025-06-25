#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>
 
// ---------- Wi-Fi ----------
#define WIFI_SSID     "ENIDH"
#define WIFI_PASSWORD "fw0SNB8J"
 
// ---------- MQTT ----------
#define MQTT_SERVER   "cjsg.ddns.net"
#define MQTT_USER     "cd"
#define MQTT_PASSWORD "1qaz\"WSX"
#define MQTT_TOPIC    "esp8266/dados"
 
// ---------- LoRa ----------
#define LORA_SS     4   // GPIO4 (D2)
#define LORA_RST    5   // GPIO5 (D1)
#define LORA_DIO0   16  // GPIO16 (D0)
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void conectarWiFi() {
  Serial.print("Conectando ao Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  unsigned long tempoInicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - tempoInicio < 20000) {
    Serial.print(".");
    delay(500);
    yield();
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? "\nWi-Fi conectado!" : "\nFalha ao conectar ao Wi-Fi.");
}
 
void conectarMQTT() {
  Serial.print("Conectando ao servidor MQTT...");
  client.setServer(MQTT_SERVER, 1883);
 
  int tentativas = 0;
  while (!client.connected() && tentativas < 5) {
    String clientId = "ESP8266Client-" + String(ESP.getChipId());
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("\nConectado ao MQTT!");
    } else {
      Serial.print("Erro MQTT (estado ");
      Serial.print(client.state());
      Serial.println("). Tentando novamente...");
      delay(3000);
      yield();
      tentativas++;
    }
  }
}
 
void setup() {
  Serial.begin(9600);
  while (!Serial);
 
  conectarWiFi();
  conectarMQTT();
 
  Serial.println("Iniciando LoRa (Receptor - ESP8266)...");
 
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
 
  if (!LoRa.begin(433E6)) {
    Serial.println("Erro ao iniciar LoRa!");
    while (1);
  }
 
  Serial.println("LoRa iniciado com sucesso. Aguardando mensagens...");
}
 
void loop() {

  if(WiFi.status()!= WL_CONNECTED){
    conectarWiFi();
  }
  if (!client.connected()) {
    conectarMQTT();
  }
  client.loop();
 
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }
    Serial.print("Mensagem recebida: ");
    Serial.println(received);
 
    client.publish(MQTT_TOPIC, received.c_str());
    Serial.println("Mensagem enviada ao MQTT!\n");
  }
}