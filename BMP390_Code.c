#include <stdint.h>
#include <stdio.h>
#include <Wire.h> //biblioteca para comunicacao I2C
#include <Adafruit_BMP3XX.h>  //biblioteca do barometro escolhido (BMP390)
#include "sensors_definition.h"


typedef struct {
    
    float pressure;    ///pressao em hpa
    float temperature;  // temperatura em C

} Measurement;

// objeto para o barômetro BMP390
Adafruit_BMP3XX bmp;

// buffer armazenamento SPIFLASH

uint8_t flash_buffer[FLASH_PAGE_SIZE];
uint16_t flash_buffer_index = 0;
uint32_t  flash_write_address = BMP_INITIAL_ADRESS; // endereco inicial na memória Flash

// funcao para inicializar os componentes

void
system_init() {

    // Inicializa o BMP390
    if (!bmp.begin_I2C()) {

        // erro ao inicializar o BMP390, entra em loop infinito
        while (1);
    }

    // inicializa a comunicacao SPI
    SPI.begin();
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); // desativa o chip de memoria SPI Flash
}

// funcao para armazenar dados na SPI Flash

void
store_data_in_flash(float pressure, float temperature) {
    Measurement data;
    data.pressure = pressure;
    data.temperature = temperature;

    // copia dados para o buffer
    memcpy(&flash_buffer[flash_buffer_index], &data, sizeof(Measurement));
    flash_buffer_index += sizeof(Measurement);

    // se o buffer estiver cheio, escreve na memoria Flash via SPI
    if (flash_buffer_index >= FLASH_PAGE_SIZE) {
        digitalWrite(FLASH_CS_PIN, LOW); // Seleciona o chip da memória Flash

        // escreve endereço na memoria Flash
        SPI.transfer(0x02); // comando de gravação (WRITE)
        SPI.transfer((flash_write_address >> 16) & 0xFF); // Endereço MSB
        SPI.transfer((flash_write_address >> 8) & 0xFF);  // Endereço médio
        SPI.transfer(flash_write_address & 0xFF);         // Endereço LSB

        // transfere dados buffer para memoria Flash
        for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++) {
            SPI.transfer(flash_buffer[i]);
        }

        digitalWrite(FLASH_CS_PIN, HIGH); 

        // atualiza o endereco de gravacao, reinicia o indice do buffer
        flash_write_address += FLASH_PAGE_SIZE;
        flash_buffer_index = 0;
    }
}
void
setup() {
    
    Serial.begin(115200);            // inicializa a comunicacao serial
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // inicializa os pinos I2C
    system_init();                   // inicializa os sensores e a memoria Flash
}
void
loop() {
    float pressure = 0.0, temperature = 0.0;

    // dados do BMP390
    if (bmp.performReading()) {
        pressure = bmp.pressure / 100.0; // Converte para hPa
        temperature = bmp.temperature;  // Temperatura em °C

        // armazena dados memoria SPI Flash
        store_data_in_flash(pressure, temperature);
    }

    // aguarda 1 segundo antes da proxima leitura
    delay(1000);
}