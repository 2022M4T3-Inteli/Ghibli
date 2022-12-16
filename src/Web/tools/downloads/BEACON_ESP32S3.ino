/* Wi-Fi FTM Responder Arduino Example
  This example code is in the Public Domain (or CC0 licensed, at your option.)
  Unless required by applicable law or agreed to in writing, this
  software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
  CONDITIONS OF ANY KIND, either express or implied.
*/
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

// SSID e senha Wifi para fazer a conexao entre a o beacon e tag
const char * WIFI_FTM_SSID = "baconzitos1";
const char * WIFI_FTM_PASS = "baconzitos1";

const char * SSID = "Inteli-COLLEGE";
const char * PWD = "QazWsx@123";

void EnviarDados()
{
  Serial.println("Conectando na rede: ");
  WiFi.begin(SSID,PWD);
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print("Tentando novamente!");
        delay(2000);
        Serial.print(".");
        WiFi.disconnect();
        delay(2000);
        WiFi.reconnect();
        delay(2000); 
      }
      postDataToServer();  
}

void postDataToServer() {

  Serial.println("Posting JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  HTTPClient http;

  http.begin("https://mn8owe-3000.preview.csb.app/registerESP/beacon");
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> doc;

  doc['name'] = "baconzitos1";
  doc['MACAdress'] = WiFi.macAddress();

  String requestBody;
  serializeJson(doc, requestBody);

  int httpResponseCode = http.POST(requestBody);

  if (httpResponseCode > 0) {

    String response = http.getString();

    Serial.println(httpResponseCode);
    Serial.println(response);
  }
}

void setup(){
  Serial.println("Iniciando o SoftAp com suporte ao FTM Responder");
  // Habilitação do AP com suporte ao FTM
  WiFi.softAP(WIFI_FTM_SSID, WIFI_FTM_PASS, 1, 0, 4, true);
}

void loop() {
  EnviarDados();
  delay(2000);
}