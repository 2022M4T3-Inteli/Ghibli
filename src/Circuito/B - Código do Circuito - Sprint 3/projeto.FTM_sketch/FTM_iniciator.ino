/* Exemplo do arduino iniciador Wi-fi FTM
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "WiFi.h"
#include <cstdlib>
/*
   THIS FEATURE IS SUPPORTED ONLY BY ESP32-S2 AND ESP32-C3
*/

// Decalaracao do SSID e a senha wi-fi
const char * WIFI_FTM_SSID = "Responder"; // SSID of AP that has FTM Enabled
const char * WIFI_FTM_PASS = "Responder@123";// senha Wifi necessaria para a conexao entre tag e beacon
float x = 40.50;// variavel de controle, necessaria para remover o valor de distancia em excecesso e informa o calculo da distancia real

// Configuraçãoes FTM
// Número de frames FTM solicitados em termos de 4 ou 8 bursts (valores permitidos - 0 (No pref), 16, 24, 32, 64)
const uint8_t FTM_FRAME_COUNT = 16;
// Período de tempo solicitado entre bursts consecutivos FTM (valores permitidos - 0 (No pref) or 2-255)
const uint16_t FTM_BURST_PERIOD = 2;

// Semáforo para sinalizar quando o relatório FTM foi recebido
xSemaphoreHandle ftmSemaphore;
// Status do relatório FTM recebido
bool ftmSuccess = true;

// manipulador de relatórios FTM com os dados calculados da viagem de ida e volta
void onFtmReport(arduino_event_t *event) {
  const char * status_str[5] = {"SUCESSO", "SEM_SUPORTE", "CONF_REJEITADA", "SEM_RESPOSTA", "FALHA"};//vetor que contém 5 possibilidades de resposta do envio do sinal
  wifi_event_ftm_report_t * report = &event->event_info.wifi_ftm_report;
  // Definição do status do relatório global
  ftmSuccess = report->status == FTM_STATUS_SUCCESS;
  if (ftmSuccess) {
    // Cálculo da distância estimada em metros
    Serial.printf("Estimativa FTM: Distancia: %.2f m, Tempo de Retorno: %u ns\n", (float)abs((report->dist_est) / 100.0 - x), report->rtt_est);
    // Ponteiro para relatório FTM com várias entradas,deve ser liberado após o uso
    free(report->ftm_report_data);
  } else {
    Serial.print("Erro FTM: ");//caso a distancia não for correspondida, ocorre o envio da mensagem erro
    Serial.println(status_str[report->status]);
  }
  
  xSemaphoreGive(ftmSemaphore);// Sinal que o relatório foi recebido
}

// Comando de inicio da sessão FTM e a espera do relatorio FTM
bool getFtmReport(){
  if(!WiFi.initiateFTM(FTM_FRAME_COUNT, FTM_BURST_PERIOD)){
    Serial.println("Erro FTM: Falha ao iniciar sessao");
    return false;
  }
  
  return xSemaphoreTake(ftmSemaphore, portMAX_DELAY) == pdPASS && ftmSuccess;//Após esperar o sinal do recebimento do relatorio,havera um retorno de que o sinal foi recebido com sucesso
}

void setup() {
  Serial.begin(115200);
  // Criação do semáforo binário(inicializado e pode ser obtido/fornecido de qualquer thread/ISR)
  ftmSemaphore = xSemaphoreCreateBinary();
  //Eventos do relatório FTM
  WiFi.onEvent(onFtmReport, ARDUINO_EVENT_WIFI_FTM_REPORT);
  
  // Conexão do AP que tem FTM habilitado
  Serial.println("Conectado ao responder");
  WiFi.begin(WIFI_FTM_SSID, WIFI_FTM_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");//enquanto não há conexão entre o iniciator e o responder, aparece uma mensagem de carregamento
  }
  Serial.println("");
  Serial.println("Wifi conectado");//comando para ter certeza que o wifi está conectado

  // Cálculos da velocidade de requisição e da frequência de propagação do iniciator para o o responder e do responder para o iniciator
  Serial.print("Iniciando a sessao com contagem de frames ");
  Serial.print(FTM_FRAME_COUNT);
  Serial.print(" e o periodo de Burst ");
  Serial.print(FTM_BURST_PERIOD * 100);
  Serial.println(" ms");

  // Solicitação de relatórios FTM até que um falhe
  while(getFtmReport());
}

void loop() {
  delay(1000);
}