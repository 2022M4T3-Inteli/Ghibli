/* Wi-Fi FTM Responder Arduino Example
  This example code is in the Public Domain (or CC0 licensed, at your option.)
  Unless required by applicable law or agreed to in writing, this
  software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
  CONDITIONS OF ANY KIND, either express or implied.
*/
#include "WiFi.h"
// SSID e senha Wifi para fazer a conexao entre a o beacon e tag
const char * WIFI_FTM_SSID = "beacon2";
const char * WIFI_FTM_PASS = "beacon2";
void setup(){
  Serial.println("Iniciando o SoftAp com suporte ao FTM Responder");
  // Habilitação do AP com suporte ao FTM
  WiFi.softAP(WIFI_FTM_SSID, WIFI_FTM_PASS, 1, 0, 4, true);
}

void loop() {
  delay(1000);
}