#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <HTTPClient.h>
#include <WiFi.h>
#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>
#include <SPI.h>


// Definições para o FTM
#define NB_APS 3          // Pontos de acesso disponíveis para conexão
#define MAX_PONTOS 3      // Número máximo de pontos de acesso que o ESP32 pode se conectar
#define DIST_PONTO_A1y 5  // Distância do ponto A1 em relação ao eixo y
#define DIST_PONTO_A3x 9  // Distância do ponto A3 em relação ao eixo x

// Definições para o RFID
#define SS_PIN 21   // porta do SDA
#define RST_PIN 14  // porta do Reset

#define SIZE_BUFFER 18     // Configuração padrão para comunicar com o RFID - O RFID tem mais 2 bits ao final dele de CRC(digito verificador)
#define MAX_SIZE_BLOCK 16  // Configuração padrão para comunicar com o RFID - Bloco de 16 bits que é o que tem o RFID


// esse objeto 'chave' é utilizado para autenticação
MFRC522::MIFARE_Key key;
// código de status de retorno da autenticação
MFRC522::StatusCode status;
// Definicoes pino modulo RC522
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Variável global do RFID
String Nome;
char UID[32] = "";
char novo[32] = "";
//char vazio[32]={"1","2","3","4","5","6",7","8","9","10","11","12",13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31","32"};

// Variavel global
int contador = 0;  // Contador para o número de pontos de acesso que o ESP32 se conectou

//Vetores com nomes de rede e senhas dos Access Points
const char* SSIDS[4] = { "SHARE-RESIDENTE", "responder1", "responder2", "responder3" };  // Inteli-COLLEGE
const char* PWD[4] = { "Share@residente", "responder1", "responder2", "responder3" };    // QazWsx@123
//String senha[4] = {"Share@residente", "responder1", "responder2", "responder3"};

//variavel global que armazena o valor do ponteiro
int Ymedio = 0;
int Xmedio = 0;

int macAdress = 0;

//variavel que diz se está ou não for da sala
String position;

//Variável que continua ou não o MENU 2
int parar = 0;

String guardaRede;

//Variável para medir a distância
int distancia[3] = { 0, 0, 0 };
float alterado[3] = {};
int indice = 0;

// Definições para o FTM
// Number of FTM frames requested in terms of 4 or 8 bursts (allowed values - 0 (No pref), 16, 24, 32, 64)
const uint8_t FTM_FRAME_COUNT = 16;
// Requested time period between consecutive FTM bursts in 100’s of milliseconds (allowed values - 0 (No pref) or 2-255)
const uint16_t FTM_BURST_PERIOD = 2;
// Semaphore to signal when FTM Report has been received
xSemaphoreHandle ftmSemaphore;
// Status of the received FTM Report
bool ftmSuccess = true;




void getDataFromServer() {

  Serial.println("Pegando dados do Servidor...");
  // Block until we are able to connect to the WiFi access point
  HTTPClient http;

  http.begin("https://0x4wxk-3000.preview.csb.app/registerESP/tags");
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> doc;

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {

    String response = http.getString();

    Serial.println(httpResponseCode);
    Serial.println(response);
    deserializeJson(doc, response);
    int action = doc["action"];
    String sensor = doc["sensor"];
    String status1 = doc["status"];
    Serial.println(action);
    Serial.println(sensor);
    Serial.println(status1);
  }
}


//int *t;// ponteiro que armazena as distancias

/*void rfid(){
  if(WiFi.status() != WL_CONNECTED){
    for(int i=0; i<31; i++){
      UID = vazio[];
    }
    novo = UID;
  }else{
    novo = UID;
  }
  //return novo;
}*/

void postDataToServer() {

  Serial.println("Posting JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  HTTPClient http;

  http.begin("https://0x4wxk-3000.preview.csb.app/registerESP/tags");
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> doc;

  // elementos do RFID para mandar para o servidor
  doc["uid"] = UID;

  //doc["uid"] = UID;


  //elementos do FTM para mandar para o servidor

  doc["position"] = position;
  doc["MACAddress"] = WiFi.macAddress();
  doc["name"] = "Matema";
  doc["area_ID"] = "48";
  doc["user_ID"] = "1234";

  String requestBody;
  serializeJson(doc, requestBody);

  int httpResponseCode = http.POST(requestBody);

  if (httpResponseCode > 0) {

    String response = http.getString();

    Serial.println(httpResponseCode);
    Serial.println(response);
  }
}


//Uma função para ler todos os dados da conexão WiFi
void DadosConexao() {
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.println("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.println("DNS IP: ");
  Serial.println(WiFi.dnsIP());
  Serial.println("BroadCast: ");
  Serial.println(WiFi.broadcastIP());
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("Network ID: ");
  Serial.println(WiFi.networkID());
  Serial.println("PSK: ");
  Serial.println(WiFi.psk());
  Serial.println("BSSID: ");
  Serial.println(WiFi.BSSIDstr());
  Serial.println("RSSI: ");
  Serial.println(WiFi.RSSI());
}


// FTM report handler with the calculated data from the round trip
void onFtmReport(arduino_event_t* event) {
  const char* status_str[5] = { "SUCCESS", "UNSUPPORTED", "CONF_REJECTED", "NO_RESPONSE", "FAIL" };
  wifi_event_ftm_report_t* report = &event->event_info.wifi_ftm_report;
  // Set the global report status
  ftmSuccess = report->status == FTM_STATUS_SUCCESS;
  if (ftmSuccess) {
    // The estimated distance in meters may vary depending on some factors (see README file)
    distancia[indice] = report->dist_est;
    Serial.printf("FTM Estimate: Distance RAW: %.4f,Distance: %.4f m, Return Time: %u ns\n", (float)report->dist_est, (float)(report->dist_est - 4000 / 1000), report->rtt_est);
    // Pointer to FTM Report with multiple entries, should be freed after use
    //free(report->ftm_report_data);
  } else {
    Serial.print("FTM Error: ");
    Serial.println(status_str[report->status]);
  }
  // Signal that report is received
  xSemaphoreGive(ftmSemaphore);
}


// Initiate FTM Session and wait for FTM Report
bool getFtmReport() {
  if (!WiFi.initiateFTM(FTM_FRAME_COUNT, FTM_BURST_PERIOD)) {
    Serial.println("FTM Error: Initiate Session Failed");
    return false;
  }
  // Wait for signal that report is received and return true if status was success
  return xSemaphoreTake(ftmSemaphore, portMAX_DELAY) == pdPASS && ftmSuccess;
}



//Função para um Menu de escolha cujo intuito é mostrar todas as possibilidades do Wifi.
//Conectar separadamente nos APs e depois fazer a triangulação
int menu() {
  Serial.println(F("\nEscolha uma opção:"));
  Serial.println(F("0 - Scan de redes"));
  Serial.println(F("1 - Conectar no beacon 1\n"));
  Serial.println(F("2 - Conectar no beacon 2\n"));
  Serial.println(F("3 - Conectar no beacon 3\n"));
  Serial.println(F("4 - Conectar nos 3 beacons (sequencialmente) \n"));
  Serial.println(F("5 - Conectar no WIFI e enviar dados para o servidor.  \n"));
  //fica aguardando enquanto o usuário nao enviar algum dado
  while (!Serial.available()) {};
  //recupera a opção escolhida
  int op = (int)Serial.read();
  //remove os proximos dados (como o 'enter ou \n' por exemplo) que vão por acidente
  while (Serial.available()) {
    if (Serial.read() == '\n') break;
    Serial.read();
  }
  return (op - 48);  //do valor lido, subtraimos o 48 que é o ZERO da tabela ascii
}


//Utilizado na função CONECTAR, para continuar mostrando os dados da conexão enquanto permanecer
//conectado Menu2.
int menu2() {
  Serial.println(F("\nEscolha uma opção 2:"));
  //fica aguardando enquanto o usuário nao enviar algum dado
  while (!Serial.available()) {};
  //recupera a opção escolhida
  int op = (int)Serial.read();
  //remove os proximos dados (como o 'enter ou \n' por exemplo) que vão por acidente
  while (Serial.available()) {
    if (Serial.read() == '\n') break;
    Serial.read();
  }
  return (op - 48);  //do valor lido, subtraimos o 48 que é o ZERO da tabela ascii
}




//Função para conectar em APs sem medição FTM
void EnviarDados(int rede) {
  Serial.println("Conectando na rede: ");
  Serial.println(rede);
  WiFi.begin(SSIDS[rede], PWD[rede]);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Tentando novamente!");
    delay(500);
    // delay(2000);
    // Serial.print(".");
    // WiFi.disconnect();
    // delay(2000);
    // WiFi.reconnect();
    // delay(2000);
  }
  while (parar == 0) {
    Serial.println("WiFi connected");
    //DadosConexao();
    postDataToServer();
    //parar = menu2();
  }
  parar = 0;
  WiFi.disconnect();
  Serial.println("Desconectei!");
}


void ReceberDados(int rede) {
  Serial.println("Conectando na rede: ");
  Serial.println(rede);
  WiFi.begin(SSIDS[rede], PWD[rede]);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Tentando novamente!");
    delay(500);
  }
  while (parar == 0) {
    Serial.println("WiFi connected");
    //DadosConexao();
    getDataFromServer();
    //parar = menu2();
  }
  parar = 0;
  WiFi.disconnect();
  Serial.println("Desconectei!");
}



//Função para conectar num AP sem medição FTM. Futuramente para conectar na internet e enviar
//os dados dos sensores
void MedirDistancia(int rede) {
  // Create binary semaphore (initialized taken and can be taken/given from any thread/ISR)
  ftmSemaphore = xSemaphoreCreateBinary();

  // Listen for FTM Report events
  WiFi.onEvent(onFtmReport, ARDUINO_EVENT_WIFI_FTM_REPORT);

  // Connect to AP that has FTM Enabled
  Serial.println("Connecting to FTM Responder");
  WiFi.begin(SSIDS[rede], PWD[rede]);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
    WiFi.disconnect();
    delay(2000);
    WiFi.reconnect();
    delay(2000);
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("Initiating FTM session with Frame Count ");
  Serial.print(FTM_FRAME_COUNT);
  Serial.print(" and Burst Period ");
  Serial.print(FTM_BURST_PERIOD * 100);
  Serial.println(" ms");
  getFtmReport();
}



// Classe ponto apenas para encapsular as coordenadas x e Y e facilitar a construção do ARRAY
class Ponto {
private:
  float coordX = 0;
  float coordY = 0;
public:
  // Constroi o ponto colocando os valores nos atributos
  Ponto(float x, float y) {
    coordX = x;
    coordY = y;
  };
  Ponto(){};  // Construtor vazio por requisição do compilador
  void put(float x, float y) {
    coordX = x;
    coordY = y;
  };
  float x() {
    return coordX;
  };
  float y() {
    return coordY;
  };
};

// Classe representa um componente que armazena os 3 pontos (nos objetos da classe Ponto)
// assim como as 3 distâncias a cada um desses pontos
// Para facilitar, chamamos os beacons de A1, A2, A3, etc mas eles são armazenados na verdade
// nos pontos 0,1, 2, etc dos respectivos vetores onde eles são armazenados
//  A2(0,y2)  |\
//            | \->dA2
//            |  \
//            |   \ b(xMedio,yMedio)
//            |   /
//            |  /->dA1
//   A1(0,0)  | /                       A3(x3,0)
//            --------------------------|-----
// Funções extras para construir o objeto sem usar o construtor

class Triangulacao {
private:
  Ponto listaPontos[MAX_PONTOS];      // Lista de objetos Ponto com as coordenadas dos 3 pontos
  float listaDistancias[MAX_PONTOS];  // Lista das distancias a cada um dos pontos A1, A2 e A3
  float yPonto_A1_A2() {              // Formula que calcula a coordenada y do Ponto B usando apenas A1 e A2
    float dA1_2 = pow(listaDistancias[0], 2);
    float dA2_2 = pow(listaDistancias[1], 2);
    float y2_2 = pow(listaPontos[1].y(), 2);
    float y2_x2 = 2 * (listaPontos[1].y());
    if (y2_x2 == 0) {
      y2_x2 = 1;
    }
    float yb = (dA1_2 - dA2_2 + y2_2) / y2_x2;
    return (yb);
  };
  float xPonto_A1_A2() {  // Formula que calcula a coordenada x do Ponto B usando apenas A1 e A2
    float dA1_2 = pow(listaDistancias[0], 2);
    float yb = yPonto_A1_A2();
    float xb = sqrt(abs(dA1_2 - yb));
    return (xb);
  };
  float yPonto_A1_A3() {  // Formula que calcula a coordenada x do Ponto B usando apenas A1 e A3
    float dA1_2 = pow(listaDistancias[0], 2);
    float dA2_2 = pow(listaDistancias[2], 2);
    float y2_2 = pow(listaPontos[2].x(), 2);
    float y2_x2 = 2 * (listaPontos[2].x());
    if (y2_x2 == 0) {
      y2_x2 = 1;
    }
    float yb = (dA1_2 - dA2_2 + y2_2) / y2_x2;
    return (yb);
  };
  float xPonto_A1_A3() {  // Formula que calcula a coordenada y do Ponto B usando apenas A1 e A3
    float dA1_2 = pow(listaDistancias[0], 2);
    float yb = yPonto_A1_A3();
    float xb = sqrt(abs(dA1_2 - yb));
    return (xb);
  };

public:
  Triangulacao(){};
  // Constroi o componente (objeto) de triangulação preenchendo a posição dos 2 beacons da ponta
  // o central é sempre (0,0) e as 3 distancias aos 3 pontos A1, A2 e A3
  Triangulacao(float yA1, float xA3, float d1, float d2, float d3) {
    adicionaPonto(0, 0, 0);
    adicionaPonto(1, 0, yA1);
    adicionaPonto(2, xA3, 0);
    putDistancia(0, d1);
    putDistancia(1, d2);
    putDistancia(2, d3);
  };

  // Funções extras para construir o objeto sem usar o construtor
  void adicionaPonto(int nr, float x, float y) {
    listaPontos[nr].put(x, y);
  };
  void putDistancia(int nrPonto, float d) {
    listaDistancias[nrPonto] = d;
  };
  /////// Calculo dos valores de x e y medios combinando os valores achados atraves de A1 e A2 com A1 e A3
  float pontoXMedio() {
    Serial.println("pontoXMedio");
    float xMedio = (xPonto_A1_A2() + xPonto_A1_A3()) / 2;
    return (xMedio);
  };
  float pontoYMedio() {
    Serial.println("pontoYMedio");
    float yMedio = (yPonto_A1_A2() + yPonto_A1_A3()) / 2;
    return (yMedio);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////
};


Triangulacao* t = NULL;


void array_to_string(byte array[], unsigned int len, char buffer[])  // Converte um array de bytes em uma string
{
  for (unsigned int i = 0; i < len; i++)  // Converte cada byte em 2 caracteres hexadecimais
  {
    byte nib1 = (array[i] >> 4) & 0x0F;                              // Extrai o nibble mais significativo
    byte nib2 = (array[i] >> 0) & 0x0F;                              // Extrai o nibble menos significativo
    buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;  // Converte o nibble para um caractere ASCII
    buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;  // Converte o nibble para um caractere ASCII
  }
  buffer[len * 2] = '\0';  // Termina a string com o caractere nulo
}

// faz a leitura dos dados do cartão/tag
void leituraDados() {
  // imprime os detalhes tecnicos do cartão/tag
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

  // Prepara a chave - todas as chaves estão configuradas para FFFFFFFFFFFFh (Padrão de fábrica).
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;  // está no datasheet, tem que colocar o FF

  // Os três seguintes parâmetros são muito importantes e tem que tomar cuidado na hora de definir eles...
  // buffer para colocar os dados lidos
  byte buffer[SIZE_BUFFER] = { 0 };  // tem que zerar ele para ler novos dados

  // bloco que faremos a operação
  byte bloco = 1;  // vai de 0 a 64, tem que definir qual bloco está usando

  byte tamanho = SIZE_BUFFER;

  // faz a autenticação do bloco que vamos operar
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &key, &(mfrc522.uid));
  //??line 834 of MFRC522.cpp file?? | Definido internamente no bloco, constante importante de usar | bloco = Tem que definir qual bloco está passando | &key = endereço da chave | UID = obrigatório tbm passar o endereço
  if (status != MFRC522::STATUS_OK) {            // status pega o método de authenticate e diz que, caso não esteja OK -->
    Serial.print(F("Authentication failed: "));  // erro de autenticação
    Serial.println(mfrc522.GetStatusCodeName(status));
    // digitalWrite(pinVermelho, HIGH);
    // delay(1000);
    // digitalWrite(pinVermelho, LOW);

    return;
  }

  // faz a leitura dos dados do bloco
  status = mfrc522.MIFARE_Read(bloco, buffer, &tamanho);  // MIFARE (tecnologia usada). O QUE É IMPORTANTE: o Bloco, o número do Buffer e o endereço do tamanho
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));  // erro na leitura
    Serial.println(mfrc522.GetStatusCodeName(status));
    // digitalWrite(pinVermelho, HIGH);
    // delay(1000);
    // digitalWrite(pinVermelho, LOW);

    return;
  }
  // else {
  //   digitalWrite(pinVerde, HIGH);
  //   delay(1000);
  //   digitalWrite(pinVerde, LOW);
  // }
  Serial.print(F("\nDados bloco ["));
  Serial.print(bloco);
  Serial.print(F("]: "));


  String str = (char*)buffer;  //transforma os dados em string para imprimir
  // Serial.println(str); // confirmação da str

  // declarar para o cliente que o máximo de caracteres para a gravação dos nomes, é 16
  for (int i = 0; i < MAX_SIZE_BLOCK; i++) {
    Nome += str[i];
  }

  // Serial.println("teste");
  Serial.println(Nome);
  Serial.println("");
}


void setup() {
  // Inicia a serial
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  SPI.begin();  // Init SPI bus

  // Inicia MFRC522
  mfrc522.PCD_Init();

  // Mensagens iniciais no serial monitor
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}



void loop() {


  int opcao = menu();
  for(int p =0; p<2; p++{
    if(p==0)
    {
      Serial.println("Rotação de beacons!");
      for (int i = 1; i < 4; i++) {
        indice = i - 1;
        MedirDistancia(i);
        delay(5000);  //******
      }
      // Aguarda a aproximacao do cartao
      if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
      }

      // Seleciona um dos cartoes
      if (!mfrc522.PICC_ReadCardSerial()) {
        return;
      }

      leituraDados();

      array_to_string(mfrc522.uid.uidByte, 4, UID);  //Insert (byte array, length, char array for output)
      Serial.println(UID);                           //Print the output uid string

      // instrui o PICC quando no estado ACTIVE a ir para um estado de "parada"
      mfrc522.PICC_HaltA();
      // "stop" a encriptação do PCD, deve ser chamado após a comunicação com autenticação, caso contrário novas comunicações não poderão ser iniciadas
      mfrc522.PCD_StopCrypto1();

      t = new Triangulacao(DIST_PONTO_A1y, DIST_PONTO_A3x, alterado[0], alterado[1], alterado[2]);


      Serial.println("Distancias:");
      for (int i = 0; i < 3; i++) {
        alterado[i] = (float)(distancia[i] - 4000) / 100;

        Serial.println(distancia[i]);
        Serial.println(alterado[i]);
      }

      Xmedio = t->pontoXMedio();
      Serial.println(Xmedio);
      Ymedio = t->pontoYMedio();
      Serial.println(Ymedio);
      //Serial.println(estado);

      if ((Xmedio >= 0 && Xmedio <= 12) && (Ymedio >= 0 && Ymedio <= 8)) {
        Serial.println("Está na sala");
        position = "In";
      } else {
        Serial.println("Está fora da sala");
        position = "Out";
      }
    }

    if(p==1){
      Serial.println("Conectar na internet e receber dados para o servidor!");
      ReceberDados(0);
    }
    
  }
  delay(5000);
}
