#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// CONFIGURAÇÕES DA REDE PRÓPRIA (AP)
const char* ap_ssid = "ByteBier_PROJET";
const char* ap_password = "12345678";
const char* endpoint = "http://192.168.4.2:5000/api/leitura";

// DEFINIÇÕES DE PINOS
#define PINO_DS18B20 5
#define PINO_DHT 4
#define PINO_RELE 23
#define PINO_BTN_MAIS 18 // Direita (Aumenta / Avança Fase)
#define PINO_BTN_MENOS 19 // Esquerda (Diminui / Recua Fase)
#define LED_WIFI 2

// VARIÁVEIS DE PROCESSO
String etapas[] = {"mostura", "fervura", "fermentacao", "maturacao"};
int etapaAtual = 2;
float tempLiq = 0, tempAmb = 0, umidade = 0, setpoint = 30.0;
bool releLigado = false;

unsigned long tempoApertoMais = 0;
unsigned long tempoApertoMenos = 0;
const int tempoTrocaFase = 2000;
unsigned long ultimaLeitura = 0;
const int intervaloEnvio = 5000;

// Objetos
OneWire oneWire(PINO_DS18B20);
DallasTemperature ds18(&oneWire);
DHT dht(PINO_DHT, DHT22);
LiquidCrystal_I2C Icd(0x27, 16, 2);

// FUNÇÃO: TROCAR ETAPA
void trocarEtapa(int direcao) {
  etapaAtual += direcao;
  if (etapaAtual > 3) etapaAtual = 0;
  if (etapaAtual < 0) etapaAtual = 3;

  if (etapas[etapaAtual] == "mostura") setpoint = 65.0;
  else if (etapas[etapaAtual] == "fervura") setpoint = 98.0;
  else if (etapas[etapaAtual] == "fermentacao") setpoint = 20.0;
  else if (etapas[etapaAtual] == "maturacao") setpoint = 2.0;

  Icd.clear();
  Icd.print("Fase Alterada:");
  Icd.setCursor(0, 1);
  Icd.print(etapas[etapaAtual]);
  delay(1500);
}

void enviarDados() {
  // Verifica se o PC está conectado na rede do ESP
  if (WiFi.softAPgetStationNum() > 0) {
    HTTPClient http;
    http.begin(endpoint);
    http.addHeader("Content-Type", "application/json");

    float tAmb = isnan(tempAmb) ? 0.0 : tempAmb;
    float tLiq = (tempLiq == -127.0 || isnan(tempLiq)) ? 0.0 : tempLiq;
    float umid = isnan(umidade) ? 0.0 : umidade;

    String json = "{\"temp_amb\":" + String(tAmb) +
                  ",\"temp_liq\":" + String(tLiq) +
                  ",\"umidade\":" + String(umid) +
                  ",\"grupo_id\":1,\"etapa\":\"" + etapas[etapaAtual] + "\"}";

    int httpResponseCode = http.POST(json);
    Serial.println("HTTP: " + String(httpResponseCode));
    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PINO_RELE, OUTPUT);
  pinMode(PINO_BTN_MAIS, INPUT_PULLUP);
  pinMode(PINO_BTN_MENOS, INPUT_PULLUP);
  pinMode(LED_WIFI, OUTPUT);
  digitalWrite(PINO_RELE, LOW);

  ds18.begin();
  dht.begin();
  Icd.init();
  Icd.backlight();

  // Inicia o Modo Access Point
  WiFi.softAP(ap_ssid, ap_password);
  Icd.print("Rede: ByteBier");
  Serial.println("Rede AP Iniciada");
}

void loop() {
  // --- LÓGICA BOTÃO MAIS (PΙΝΟ 18) ---
  if (digitalRead(PINO_BTN_MAIS) == LOW) {
    if (tempoApertoMais == 0) tempoApertoMais = millis();
    if (tempoApertoMais > 0 && (millis() - tempoApertoMais > tempoTrocaFase)) {
      trocarEtapa(1);
      tempoApertoMais = 0;
      while(digitalRead(PINO_BTN_MAIS) == LOW);
    }
  } else {
    if (tempoApertoMais > 0 && (millis() - tempoApertoMais < 500)) {
      setpoint += 0.5;
    }
    tempoApertoMais = 0;
  }

  // --- LÓGICA BOTÃO MENOS (PINO 19) ---
  if (digitalRead(PINO_BTN_MENOS) == LOW) {
    if (tempoApertoMenos == 0) tempoApertoMenos = millis();
    if (tempoApertoMenos > 0 && (millis() - tempoApertoMenos > tempoTrocaFase)) {
      trocarEtapa(-1);
      tempoApertoMenos = 0;
      while(digitalRead(PINO_BTN_MENOS) == LOW);
    }
  } else {
    if (tempoApertoMenos > 0 && (millis() - tempoApertoMenos < 500)) {
      setpoint -= 0.5;
    }
    tempoApertoMenos = 0;
  }

  // --- LEITURAS E ENVIO ---
  if (millis() - ultimaLeitura > intervaloEnvio) {
    ultimaLeitura = millis();
    ds18.requestTemperatures();
    tempLiq = ds18.getTempCByIndex(0);
    tempAmb = dht.readTemperature();
    umidade = dht.readHumidity();

    releLigado = (tempLiq > setpoint + 0.5);
    digitalWrite(PINO_RELE, releLigado ? HIGH : LOW);

    Icd.clear();
    Icd.print("L:"); Icd.print(tempLiq, 1); Icd.print(" S:"); Icd.print(setpoint, 1);
    Icd.setCursor(0, 1);
    Icd.print(etapas[etapaAtual]);
    Icd.print(releLigado ? " ON" : " OFF");

    enviarDados();

    // O LED acende se houver alguém conectado na rede do ESP
    digitalWrite(LED_WIFI, WiFi.softAPgetStationNum() > 0);
  }
}