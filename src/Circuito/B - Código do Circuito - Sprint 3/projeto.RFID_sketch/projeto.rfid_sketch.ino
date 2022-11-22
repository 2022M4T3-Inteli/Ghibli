// Refazendo de acordo com a nova modulação
// Analisar a questão de mudança de leitura e gravação  para somente leitura.
// Conexão do Esp e RFID está Ok - só precisa comentar a parte de gravar
// Incluir mais um LED - Que é o LED amarelo
    //Aleterar as funcionalidadedes do LED
// incluir a funcionalidade de temporizador do botão
// LCD: Gravar as frases e função do LCD - integrando com as cores de LED
    //  Olhar fotos no celular.
// TENTAR AO FINAL DE TUDO, CONECTAR AO NODE - FALAR COM O SERGIO.


#include <iostream> // Biblioteca para o C++
#include <deprecated.h>
#include <MFRC522.h> //biblioteca principal
#include <MFRC522Extended.h> // extensão da biblioteca principal
#include <require_cpp11.h> //iremos usar o C++ versão 11
//#include <mfrc522.h> //biblioteca responsável pela comunicação com o módulo RFID-RC522
#include <LiquidCrystal_I2C.h> //biblioteca responsável pela comunicação com o LCD
#include <SPI.h> //biblioteca para comunicação do barramento SPI = Comunica com o RFID


// Definição dos pinos de comunicação do módulo RFID-RC522
#define SS_PIN    21 // porta do SDA
#define RST_PIN   14 // porta do Reset
//Descobrir pino MISO: 13
//Descobrir pino MOSI: 11
//Descobrir pino SCK: 12
#define SIZE_BUFFER     18 // Configuração padrão para comunicar com o RFID - O RFID tem mais 2 bits ao final dele de CRC(digito verificador)
#define MAX_SIZE_BLOCK  16 // Configuração padrão para comunicar com o RFID - Bloco de 16 bits que é o que tem o RFID

//esse objeto 'chave' é utilizado para autenticação (RFID)
MFRC522::MIFARE_Key key;

//código de status de retorno da autenticação (RFID)
MFRC522::StatusCode status;

// Definicoes pino modulo RC522 (RFID)
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Definição dos pinos de comunicação do módulo LCD
#define SDA_PIN  17// porta do SDA
#define SCL_PIN 16// porta do SCL
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Definição dos pinos de comunicação com os LED's
#define RED 1 // colocar um led de verificação
#define YELLOW 15 // colocar um led de verificação
#define GREEN 2 // colocar um led de verificação
int CurrentLight = 0; // variável para armazenar o estado atual do LED

// Definição do pino de comunicação do botão
#define Button 18
int buttonState = 0;

int Verify = 0;
int Authorized = 0;
int Inside = 0;
int Start = 0;

int milisegundos = 0;
int segundos = 0;
int minutos = 0;
int horas = 0;


void setup() {
  // Inicia a serial
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(Button, INPUT);
  
  Wire.begin(SDA_PIN, SCL_PIN); // Inicia a comunicação com o LCD
  lcd.init(); // Inicia o LCD
  lcd.backlight(); // Liga o LCD
  
  // Inicia MFRC522
  mfrc522.PCD_Init();
  
  // Mensagens iniciais no serial monitor
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
}

void LightState (int CurrentLight) { // Função para verificar o estado do LED
    if (CurrentLight == 0) { // Se o estado do LED for 0
        digitalWrite(RED, HIGH); // Liga o LED vermelho
        digitalWrite(YELLOW, LOW); // Não liga o LED amarelo
        digitalWrite(GREEN, LOW); // Não liga o LED verde
    }
    else if (CurrentLight == 1) { // Se o estado do LED for 1
        digitalWrite(RED, LOW); // Não liga o LED vermelho
        digitalWrite(YELLOW, HIGH); // Liga o LED amarelo
        digitalWrite(GREEN, LOW); // Não liga o LED verde
    }
    else if (CurrentLight == 2) { // Se o estado do LED for 2
        digitalWrite(RED, LOW); // Não liga o LED vermelho
        digitalWrite(YELLOW, LOW); // Não liga o LED amarelo
        digitalWrite(GREEN, HIGH); // Liga o LED verde
    }
}

void Count(int Start) { // Função para contar o tempo
    if (Start == 1) { // Se o estado do botão for 1
        milisegundos = millis(); // Conta os milisegundos
        if (milisegundos >= 1000) { // Se os milisegundos forem maiores ou iguais a 1000
            segundos++; // Adiciona 1 segundo
            milisegundos = 0; // Zera os milisegundos
        }
        if (segundos >= 60) { // Se os segundos forem maiores ou iguais a 60
            minutos++; // Adiciona 1 minuto
            segundos = 0; // Zera os segundos
        }
        if (minutos >= 60) { // Se os minutos forem maiores ou iguais a 60
            horas++; // Adiciona 1 hora
            minutos = 0; // Zera os minutos
        }
    }
    char time[50]; // Cria um vetor de caracteres para armazenar o tempo
    lcd.setCursor(5,0); // Posiciona o cursor na coluna 5 e linha 0
    lcd.println("Tempo"); // Imprime a palavra "Tempo" no LCD
    sprintf(time, "%02d:%02d:%02d",horas, minutos, segundos); // Formata o tempo para HH:MM:SS
    lcd.setCursor(4,1); // Posiciona o cursor na coluna 4 e linha 1
    lcd.println(time); // Imprime o tempo no LCD
    delay(1000);
    
    if (Start == 2) { // Se o estado do botão for 2
    milisegundos = milisegundos; // Mantém os milisegundos
    segundos = segundos;  // Mantém os segundos
    minutos = minutos; // Mantém os minutos
    horas = horas; // Mantém as horas
    
    char time[50]; // Cria um vetor de caracteres para armazenar o tempo
    lcd.setCursor(5,0); // Posiciona o cursor na coluna 5 e linha 0
    lcd.println("Tempo"); // Imprime a palavra "Tempo" no LCD
    sprintf(time, "%02d:%02d:%02d",horas, minutos, segundos); // Formata o tempo para HH:MM:SS
    lcd.setCursor(4,1); // Posiciona o cursor na coluna 4 e linha 1
    lcd.println(time); // Imprime o tempo no LCD
  }
}

//faz a leitura dos dados do cartão/tag
void leituraDados() {
  Serial.println(F("vini é brabo"));
  //imprime os detalhes tecnicos do cartão/tag
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
  
  //Prepara a chave - todas as chaves estão configuradas para FFFFFFFFFFFFh (Padrão de fábrica).
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; // está no datasheet, tem que colocar o FF
  // Os três seguintes parâmetros são muito importantes e tem que tomar cuidado na hora de definir eles...
  
  //buffer para colocar os dados lidos
  byte buffer[SIZE_BUFFER] = {0}; //tem que zerar ele para ler novos dados
  
  //bloco que faremos a operação
  byte bloco = 1; //vai de 0 a 64, tem que definir qual bloco está usando
  byte tamanho = SIZE_BUFFER;
  //faz a autenticação do bloco que vamos operar
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloco, &key, &(mfrc522.uid));
  //??line 834 of MFRC522.cpp file?? | Definido internamente no bloco, constante importante de usar | bloco = Tem que definir qual bloco está passando | &key = endereço da chave | UID = obrigatório tbm passar o endereço
  
  if (status != MFRC522::STATUS_OK) { //status pega o método de authenticate e diz que, caso não esteja OK -->
    lcd.clear();
    lcd.println("Erro de");
    lcd.setCursor(0, 1);
    lcd.println("autenticacao...");
    delay(2000);
    
    // Serial.print(F("Autenticacao falhou: ")); //erro de autenticação
    lcd.clear();
    // lcd.println(mfrc522.(MFRC522::StatusCode)status));
    delay(2000);
    // Serial.println(mfrc522.(MFRC522::StatusCode)status)); //mostra o erro
    digitalWrite(RED, HIGH);
    delay(1000);
    digitalWrite(RED, LOW);
  }
  
  //faz a leitura dos dados do bloco
  status = mfrc522.MIFARE_Read(bloco, buffer, &tamanho); //MIFARE (tecnologia usada). O QUE É IMPORTANTE: o Bloco, o número do Buffer e o endereço do tamanho
    if (status == MFRC522::STATUS_OK && status == Authorized && Authorized == 0) { //se estiver OK, mostra os dados lidos no LCD
      lcd.clear();
      LightState(1);
      lcd.println("Checando");
      lcd.setCursor(0,1);
      lcd.println("Credenciais...");
      delay(2000);
      
      lcd.clear();
      LightState(2);
      lcd.println("Credenciais");
      lcd.setCursor(0,1);
      lcd.println("Reconhecidas!");
      delay(1000);
      
      lcd.clear();
      Serial.print(F("\nDados bloco ["));
      Serial.print(bloco);Serial.print(F("]: "));
      lcd.println("Bem Vindo!");
      lcd.setCursor(0,1);
      for (uint8_t i = 0; i < MAX_SIZE_BLOCK; i++) // para imprimir o dado lido, tem que varrer/limpar o array com algo, sendo isso o MAX - que nada mais é, os 16 bits
      Serial.write(buffer[i]);
      Serial.println(" ");

      lcd.clear();
      Authorized = 1;
      CurrentLight = 2;
    } else if (status == MFRC522::STATUS_OK && status == Authorized && Authorized == 3)
    {
      lcd.clear();
      LightState(1);
      lcd.println("Checando");
      lcd.setCursor(0,1);
      lcd.println("Credenciais...");
      delay(2000);
      
      lcd.clear();
      LightState(2);
      lcd.println("Credenciais");
      lcd.setCursor(0,1);
      lcd.println("Reconhecidas!");
      delay(1000);
      
      lcd.clear();
      LightState(0);
      lcd.println("Desconectado");
      lcd.setCursor(0,1);
      lcd.println("com Sucesso!");
      delay(2000);

      lcd.clear();
      Authorized = 0;
      CurrentLight = 0;
      Start = 0;
    }
  Verify = 0;
    
    if (status != MFRC522::STATUS_OK) { //se o status de leitura estiver diferente de OK -->
      lcd.clear();
      lcd.println("Erro na");
      lcd.setCursor(0, 1);
      lcd.println("leitura...");
      delay(2000);
      Serial.print(F("Leitura Falhou: ")); //erro na leitura
      
      lcd.clear();
      // lcd.println(mfrc522.(MFRC522::StatusCode)status));
      delay(2000);
      Serial.println(mfrc522.GetStatusCodeName((MFRC522::StatusCode)status));
      
      digitalWrite(RED, HIGH);
      delay(1000);
      digitalWrite(RED, LOW);
      return;
    }
}

void Position() {

    if (Authorized == 1) {

    lcd.clear();
    lcd.println("Cronometragem");
    lcd.setCursor(0,1);
    lcd.println("Iniciada.");
    delay(2000); 

    lcd.clear();
    Start = 1;
    Authorized = 2;

  } else if (Authorized == 2) {

    lcd.clear();
    lcd.println("Fora do");
    lcd.setCursor(0,1);
    lcd.println("Ambiente.");
    delay(2000);

    lcd.clear();
    LightState(1);
    CurrentLight = 1; 
    Authorized = 3;
    Start = 2 ;

  } else if (Authorized == 3) {

    lcd.clear();
    lcd.println("Contagem");
    lcd.setCursor(0,1);
    lcd.println("Retomada.");
    delay(1000);

    lcd.clear();
    Start = 1;
    Authorized = 2;
    LightState(2);
    CurrentLight = 2;
  }
  Inside = 0;
}

int menu() {
  Serial.println(F("Lendo cartão:"));

  while(!Serial.available());
}

void loop() {
    // Aguarda a aproximacao do cartao
  if ( ! mfrc522.PICC_IsNewCardPresent()){
    return; 
  }
  // Seleciona o cartão aproximado e lê as especificações
  if ( ! mfrc522.PICC_ReadCardSerial()){
    return;
  }

  int Leitura = menu();
  if (Leitura == 0)
  leituraDados();
  else {
    return;
  }  

  buttonState = digitalRead(Button);

  Count(Start);
  LightState(CurrentLight);

  if (buttonState == HIGH) {
    Inside = 1;
    Position();
  }
  if (Inside == 1) {
    leituraDados();
  }

  // instrui o PICC quando no estado ACTIVE a ir para um estado de "parada"/standBy
  mfrc522.PICC_HaltA();
  // "stop" a encriptação do PCD, deve ser chamado após a comunicação com autenticação, caso contrário novas comunicações não poderão ser iniciadas
  mfrc522.PCD_StopCrypto1();
}