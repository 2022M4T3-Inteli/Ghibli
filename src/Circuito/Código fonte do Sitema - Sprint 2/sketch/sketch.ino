// Link para o WokWi do Projeto: https://wokwi.com/projects/347407088667853396

// bibliotecas 
#include <iostream>
#include <LiquidCrystal_I2C.h>

// Definindo as portas referentes do Esp32-S3, tanto do simulador quanto do físico
#define SDA_PIN 9
#define SCL_PIN 10
// definindo especificações do display
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Definindo as portas referentes do Esp32-S3, tanto do simulador quanto do físico, além de algumas variáveis
const int RED = 12;
const int YELLOW = 13;
const int GREEN = 18;
int CurrentLight = 0;

const int AUTH = 1;
const int WALK = 2;
int authState = 0;
int walkState = 0;
int Verify = 0;
int Authorized = 0;
int Inside = 0;

const int BUZZ = 20;

// definindo algumas variáveis
int Start = 0;
int milisegundos = 0;
int segundos = 0;
int minutos = 0;
int horas = 0;

// Setup do código, onde definimos o que é OUTPUT e INPUT
void setup(){
  Serial.begin(115200);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(AUTH, INPUT);
  pinMode(WALK, INPUT);
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
}

// Loop do código onde chamaremos as funções e iremos implementar toda a aplicação do sistema
void loop() {

  authState = digitalRead(AUTH);
  walkState = digitalRead(WALK);

  Count(Start); 
  LightState(CurrentLight);

  if (authState == LOW) {
    Verify = 1; 
  }

  if (authState == HIGH && Verify == 1) {
    Verification();
  }

  if (walkState == LOW) {
    Inside = 1;
  }

  if (walkState == HIGH && Inside == 1) {
    Position();
  }

}

// Função referente ao estado dos LED's
void LightState (int CurrentLight) {
    if (CurrentLight == 0) {
    digitalWrite(RED, 1);
    digitalWrite(GREEN, 0);
    digitalWrite(YELLOW, 0);
    } else if (CurrentLight == 1) {
    digitalWrite(YELLOW, 1);
    digitalWrite(RED, 0);
    digitalWrite(GREEN, 0);
    } else if (CurrentLight == 2) {
    digitalWrite(GREEN, 1);
    digitalWrite(YELLOW, 0);
    digitalWrite(RED, 0);
    }
}

// Função referente ao tempo que procede todas as funções do sistema, desde ligar Led à aparição de informação no Display
void Count(int Start) {
  if (Start == 0) {

    milisegundos = 0;
    segundos = 0; 
    minutos = 0;
    horas = 0;


  } else if (Start == 1) {

    milisegundos = millis();
    if (milisegundos >= 1000) {
      segundos++;
      milisegundos = 0;
    }
    if (segundos / 60 == 1) {
      minutos++;
      segundos = 0;
    }
    if (minutos / 60 == 1) {
      horas++;
      minutos = 0;
    }

    char time[50];

    lcd.setCursor(5,0);

    lcd.println("Tempo");

    sprintf(time, "%02d:%02d:%02d",horas, minutos, segundos);

    lcd.setCursor(4,1);
    lcd.println(time);

    delay(1000); 

  } else if (Start == 2) {
    milisegundos = milisegundos;
    segundos = segundos; 
    minutos = minutos;
    horas = horas;

    char time[50];

    lcd.setCursor(5,0);

    lcd.println("Tempo");

    sprintf(time, "%02d:%02d:%02d",horas, minutos, segundos);

    lcd.setCursor(4,1);
    lcd.println(time);
  }
}

// Função de verificação das informações que serão apresentadas no display, além do estado que se mostra o LED
void Verification() {

  if (Authorized == 0) {

    lcd.clear();
    LightState(1);
    lcd.println("Checando");
    lcd.setCursor(0,1);
    lcd.println("Credenciais...");
    tone(BUZZ, 1000, 250);
    delay(1500);

    lcd.clear();
    LightState(2);
    tone(BUZZ, 1000, 500);
    lcd.println("Credenciais");
    lcd.setCursor(0,1);
    lcd.println("Reconhecidas!");
    delay(500);

    lcd.clear();
    lcd.println("Bem Vinda");
    lcd.setCursor(0,1);
    lcd.println("Thaina Lima!");
    tone(BUZZ, 1000, 2000);
    delay(2000);

    lcd.clear();
    Authorized = 1;
    CurrentLight = 2;

  } else if (Authorized == 3) {

    lcd.clear();
    LightState(1);
    lcd.println("Checando");
    lcd.setCursor(0,1);
    lcd.println("Credenciais...");
    tone(BUZZ, 1000, 250);
    delay(1500);

    lcd.clear();
    LightState(2);
    tone(BUZZ, 1000, 500);
    lcd.println("Credenciais");
    lcd.setCursor(0,1);
    lcd.println("Reconhecidas!");
    delay(500);

    lcd.clear();
    LightState(0);
    lcd.println("Desconectado");
    lcd.setCursor(0,1);
    lcd.println("com Sucesso!");
    tone(BUZZ, 1000, 2000);
    delay(2000);

    lcd.clear();
    Authorized = 0;
    CurrentLight = 0;
    Start = 0;
  }
  Verify = 0;
}

// Função de posição em que se encontra a pessoa/tag em referência ao ambiente em que ela está
// Então, ao apertar o botão verde, reconhce a pessoa e ao apertar o botão azul, começa a cronometragem pelo fato da pessoa estar naquele determinado ambiente
void Position() {

  if (Authorized == 0) {

    lcd.clear();
    lcd.println("Falha na");
    lcd.setCursor(0,1);
    lcd.println("Credencial");
    tone(BUZZ, 1000, 2000);
    delay(2000);
    lcd.clear();

  } else if (Authorized == 1) {

    lcd.clear();
    lcd.println("Cronometragem");
    lcd.setCursor(0,1);
    lcd.println("Iniciada.");
    tone(BUZZ, 1000, 1000);
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