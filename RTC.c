#include <Wire.h>
#include <RTClib.h> // Biblioteca para o RTC DS3231
#include "sensors_definition.h"

// Estrutura para armazenar os dados na memória Flash
typedef struct {
    uint8_t year;     // Ano (2 bytes)
    uint8_t month;    // Mês (1 byte)
    uint8_t day;      // Dia (1 byte)
    uint8_t hour;     // Hora (1 byte)
    uint8_t minute;   // Minuto (1 byte)
    uint8_t second;   // Segundo (1 byte)
} RTCData;

// Objeto para o RTC
RTC_DS3231 rtc;

// Buffer para armazenamento na Flash
uint8_t flash_buffer[FLASH_PAGE_SIZE];
uint16_t flash_buffer_index = 0;
uint32_t flash_write_address = RTC_INITIAL_ADDRESS; // Endereço inicial na memória Flash

// Função para inicializar os componentes
void system_init() {
    // Inicializa o RTC
    if (!rtc.begin()) {
        // Se não encontrar o RTC, entra em loop infinito
        while (1);
    }

    // Inicializa a comunicação SPI
    SPI.begin();
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); // Desativa o chip de memória SPI Flash
}

// Função para armazenar os dados do RTC na memória Flash
void store_rtc_data_in_flash(DateTime now) {
    RTCData data;
    data.year = now.year() - 2000; // Ano (2 dígitos)
    data.month = now.month();
    data.day = now.day();
    data.hour = now.hour();
    data.minute = now.minute();
    data.second = now.second();

    // Copia os dados para o buffer
    memcpy(&flash_buffer[flash_buffer_index], &data, sizeof(RTCData));
    flash_buffer_index += sizeof(RTCData);

    // Se o buffer estiver cheio, escreve na memória Flash via SPI
    if (flash_buffer_index >= FLASH_PAGE_SIZE) {
        digitalWrite(FLASH_CS_PIN, LOW); // Seleciona o chip da memória Flash

        // Escreve o endereço na memória Flash
        SPI.transfer(0x02); // Comando de gravação (WRITE)
        SPI.transfer((flash_write_address >> 16) & 0xFF); // Endereço MSB
        SPI.transfer((flash_write_address >> 8) & 0xFF);  // Endereço médio
        SPI.transfer(flash_write_address & 0xFF);         // Endereço LSB

        // Transfere os dados do buffer para a memória Flash
        for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++) {
            SPI.transfer(flash_buffer[i]);
        }

        digitalWrite(FLASH_CS_PIN, HIGH); // Desativa o chip da memória Flash

        // Atualiza o endereço de gravação e reinicia o índice do buffer
        flash_write_address += FLASH_PAGE_SIZE;
        flash_buffer_index = 0;
    }
}

void setup() {
    Serial.begin(115200);               // Inicializa a comunicação serial
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // Inicializa os pinos I2C
    system_init();                      // Inicializa o RTC e a memória Flash
}

void loop() {
    // Obtém o horário atual do RTC
    DateTime now = rtc.now();

    // Armazena os dados na memória Flash
    store_rtc_data_in_flash(now);

    // Imprime no monitor serial (para debug)
    Serial.print("Hora atual: ");
    Serial.print(now.year(), DEC);
    Serial.print("/");
    Serial.print(now.month(), DEC);
    Serial.print("/");
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print(":");
    Serial.println(now.second(), DEC);

    // Aguarda 1 segundo antes da próxima leitura
    delay(1000);
}
