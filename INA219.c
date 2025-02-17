#include <stdint.h>
#include <stdio.h>
#include <Adafruit_INA219.h>
#include "sensors_definition.h"


typedef struct {

  float current; //corrente em mA
  float voltage; //voltagem em V

} measures; 

// Inicializa o objeto Adafruit_INA219
Adafruit_INA219 ina219;






// Iniciando a comunicação serial  
void setup() {

 Serial.begin(115200);
  while (!Serial) {
      // will pause until serial console opens
      delay(1);
  };
};






// Inicializar componentes
void start() {

  // Start INA219: se a operação de inicializar o ina219 não funcionar entrará em um loop e dará um erro 
  if (!ina219.begin()) {

     printf("Failed to find INA219 chip");
      while (1) { delay(10);};
  };

  printf("INA219 chip found");

// inicializa a comunicacao SPI
    SPI.begin();
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); // desativa o chip de memoria SPI Flash


  printf("Measuring voltage and current with INA219 ...");
};





//Função para coletar dados medidos pelo sensor
void loop() {
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float power_mW = 0; 
  float loadvoltage = 0;
  
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");

  delay(1000); //Intervalo de 1 segundo para fazer a próxima leitura
}

