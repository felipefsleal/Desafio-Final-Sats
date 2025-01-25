#include <Arduino.h>
#include <math.h>

const int sensorPin = A0;  // Pino analógico para o sensor TEMT6000
const int ledPin = 9;      // Pino digital para o LED UYG

float leituraReferencia = 0;  // Intensidade de luz sem solução
float leituraAmostra = 0;     // Intensidade de luz com solução

void setup() {
  Serial.begin(9600);          // Inicializa a comunicação serial
  pinMode(ledPin, OUTPUT);     // Configura o pino do LED como saída
  digitalWrite(ledPin, LOW);   // LED desligado inicialmente
  Serial.println("Espectrofotômetro iniciado");
}

void loop() {
  // Passo 1: Leitura da luz de referência (sem a amostra, apenas o indicador)
  Serial.println("Posicione o sensor para a leitura de referência e pressione ENTER.");
  while (Serial.available() == 0) {}  // Aguarda o pressionamento de ENTER
  Serial.read();  // Limpa o buffer

  digitalWrite(ledPin, HIGH);   // Liga o LED
  delay(500);                   // Aguarda estabilização
  leituraReferencia = analogRead(sensorPin);  // Lê o valor do sensor
  digitalWrite(ledPin, LOW);    // Desliga o LED

  Serial.print("Leitura de referência: ");
  Serial.println(leituraReferencia);

  // Passo 2: Leitura da luz transmitida pela amostra com o indicador Alamar Blue
  Serial.println("Insira a amostra e pressione ENTER.");
  while (Serial.available() == 0) {}  // Aguarda o pressionamento de ENTER
  Serial.read();  // Limpa o buffer

  digitalWrite(ledPin, HIGH);   // Liga o LED
  delay(500);                   // Aguarda estabilização
  leituraAmostra = analogRead(sensorPin);  // Lê o valor do sensor
  digitalWrite(ledPin, LOW);    // Desliga o LED

  Serial.print("Leitura da amostra: ");
  Serial.println(leituraAmostra);

  // Passo 3: Cálculo da absorção com base nas leituras
  if (leituraAmostra > 0 && leituraReferencia > 0) {
    float absorcao = -log10(leituraAmostra / leituraReferencia);  // Cálculo da absorção
    Serial.print("Absorção calculada: ");
    Serial.println(absorcao, 4);  // Exibe a absorção com 4 casas decimais
  } else {
    Serial.println("Erro: Leitura de amostra ou referência inválida (valor zero).");
  }

  delay(2000);  // Pausa antes da próxima iteração
}
