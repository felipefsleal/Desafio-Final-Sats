#include <stdint.h>
#include <stdio.h>
#include <SPI.h> //biblioteca para comunicacao SPI
#include <Adafruit_BNO055.h> //biblioteca do UMI escolhido
#include "sensors_definition.h"

typedef struct {
    float orientation[3]; // orientação em graus (pitch, roll, yaw)
    float acceleration[3]; // aceleração em m/s^2 (x, y, z)
} Measurement;

// objeto para o UMI BNO055
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &SPI, 10); // 10 é o CS pin

// buffer armazenamento SPIFLASH
uint8_t flash_buffer[FLASH_PAGE_SIZE];
uint16_t flash_buffer_index = 0;
uint32_t flash_write_address = 0x000000; // endereco inicial na memória Flash

// funcao para inicializar os componentes
void system_init() {
    // Inicializa o BNO055
    if (!bno.begin()) {
        // erro ao inicializar o BNO055, entra em loop infinito
        while (1);
    }

    // inicializa a comunicacao SPI
    SPI.begin();
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); // desativa o chip de memoria SPI Flash
}

// funcao para armazenar dados na SPI Flash
void store_data_in_flash(float orientation[3], float acceleration[3]) {
    Measurement data;
    for (int i = 0; i < 3; i++) {
        data.orientation[i] = orientation[i];
        data.acceleration[i] = acceleration[i];
    }

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

void setup() {
    Serial.begin(115200);            // inicializa a comunicacao serial
    system_init();                   // inicializa os sensores e a memoria Flash
}

void loop() {
    float orientation[3] = {0.0, 0.0, 0.0};
    float acceleration[3] = {0.0, 0.0, 0.0};

    // dados do BNO055
    sensors_event_t event;
    bno.getEvent(&event);

    orientation[0] = event.orientation.x;
    orientation[1] = event.orientation.y;
    orientation[2] = event.orientation.z;

    acceleration[0] = event.acceleration.x;
    acceleration[1] = event.acceleration.y;
    acceleration[2] = event.acceleration.z;

    // armazena dados memoria SPI Flash
    store_data_in_flash(orientation, acceleration);

    // aguarda 1 segundo antes da proxima leitura
    delay(1000);
}