#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* servidorURL = "http://192.168.4.1/ping";

// Calibre estes dois valores medindo o RSSI com os ESPs a 1 m
float txPower = -47.0;  // RSSI medido a 1 m
float n = 2.0;          // expoente de perda (2 = espaco livre, 2.5 a 4 em ambiente interno)

float estimarDistancia(long rssi) {
  float expoente = (txPower - rssi) / (10.0 * n);
  return pow(10.0, expoente);
}

String classificarProximidade(float distancia) {
  if (distancia < 1.5)
    return "PERTO";
  else if (distancia < 5.0)
    return "MEDIO";
  else
    return "LONGE";
}

void setup() {
  Serial.begin(115200);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin("ESP_Referencia", "senha123");

  Serial.println();
  Serial.print("Conectando");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Conectado ao servidor!");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexao perdida, aguardando...");
    delay(1000);
    return;
  }

  long rssi = WiFi.RSSI();
  float distancia = estimarDistancia(rssi);

  // Teste de vida do servidor
  WiFiClient client;
  HTTPClient http;
  http.begin(client, servidorURL);
  int httpCode = http.GET();
  http.end();

  Serial.print("RSSI: ");
  Serial.print(rssi);
  Serial.print(" dBm | Distancia: ");
  Serial.print(distancia, 2);
  Serial.print(" m | ");
  Serial.print(classificarProximidade(distancia));
  Serial.print(" | servidor: ");
  Serial.println(httpCode == 200 ? "ok" : "sem resposta");

  delay(1000);
}
