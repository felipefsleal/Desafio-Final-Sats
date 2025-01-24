#include <stdint.h>
#include <stdio.h>
#include <Wire.h> // Biblioteca para comunicação I2C
#include <Adafruit_MCP9808.h> // Biblioteca para o sensor de temperatura MCP9808
#include "sensors_definition.h" // Definições dos pinos e memória Flash

typedef struct {
    float temperature; // Temperatura em °C
} Measurement;

// Objeto para o sensor de temperatura MCP9808
Adafruit_MCP9808 tempSensor;

// Buffer para armazenamento na memória SPI Flash
uint8_t flash_buffer[FLASH_PAGE_SIZE];
uint16_t flash_buffer_index = 0;
uint32_t flash_write_address = MCP_INITIAL_ADDRESS; // Endereço inicial na memória Flash

// Função para inicializar os componentes
void system_init() {
    // Inicializa o MCP9808
    if (!tempSensor.begin(0x18)) { // Endereço padrão I2C do MCP9808
        // Erro ao inicializar o sensor, entra em loop infinito
        while (1);
    }

    // Inicializa a comunicação SPI
    SPI.begin();
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); // Desativa o chip de memória SPI Flash
}

// Função para armazenar dados na memória SPI Flash
void store_data_in_flash(float temperature) {
    Measurement data;
    data.temperature = temperature;

    // Copia os dados para o buffer
    memcpy(&flash_buffer[flash_buffer_index], &data, sizeof(Measurement));
    flash_buffer_index += sizeof(Measurement);

    // Se o buffer estiver cheio, escreve na memória Flash via SPI
    if (flash_buffer_index >= FLASH_PAGE_SIZE) {
        digitalWrite(FLASH_CS_PIN, LOW); // Seleciona o chip da memória Flash

        // Comando para gravar na memória Flash
        SPI.transfer(0x02); // Comando de gravação (WRITE)
        SPI.transfer((flash_write_address >> 16) & 0xFF); // Endereço MSB
        SPI.transfer((flash_write_address >> 8) & 0xFF);  // Endereço médio
        SPI.transfer(flash_write_address & 0xFF);         // Endereço LSB

        // Transfere os dados do buffer para a memória Flash
        for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++) {
            SPI.transfer(flash_buffer[i]);
        }

        digitalWrite(FLASH_CS_PIN, HIGH); // Desativa o chip de memória Flash

        // Atualiza o endereço de gravação e reinicia o índice do buffer
        flash_write_address += FLASH_PAGE_SIZE;
        flash_buffer_index = 0;
    }
}

void setup() {
    Serial.begin(115200);            // Inicializa a comunicação serial
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // Inicializa os pinos I2C
    system_init();                   // Inicializa o sensor e a memória Flash
}

void loop() {
    float temperature = 0.0;

    // Lê os dados do MCP9808
    temperature = tempSensor.readTempC();

    // Armazena os dados na memória SPI Flash
    store_data_in_flash(temperature);

    // Aguarda 1 segundo antes da próxima leitura
    delay(1000);
}
