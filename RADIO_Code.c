#include <stdint.h>
#include <SPI.h>
#include <RH_RF95.h>
#include "sensors_definition.h"

#define RADIO_FREQ 915.0 // frequencia do radio MHz
#define RFM95_INT_PIN 13 // pino init radio (interromper)

// buffer para leitura da memoria flash
uint8_t flash_buffer[FLASH_PAGE_SIZE];
uint32_t flash_read_address = 0x000000; // endereco inicial da memoria Flash

// objeto do rádio
RH_RF95 rf95(RADIO_CS_PIN, RFM95_INT_PIN);

// funcao para inicializar o radio
void initRadio() {
    if (!rf95.init()) {
        while (1);
    }
    rf95.setFrequency(RADIO_FREQ);
    rf95.setTxPower(23, false);
}

// funcao para ler dados da memoria Flash
bool read_data_from_flash(uint32_t address, uint16_t length) {
    if (address + length > 0xFFFFF) {
        return false; // endereco fora dos limites da memoria
    }

    memset(flash_buffer, 0, FLASH_PAGE_SIZE);

    digitalWrite(FLASH_CS_PIN, LOW);

    SPI.transfer(0x03); // comando leitura
    SPI.transfer((address >> 16) & 0xFF); // endereco MSB
    SPI.transfer((address >> 8) & 0xFF);
    SPI.transfer(address & 0xFF);         // endereco LSB

    for (uint16_t i = 0; i < length; i++) {
        flash_buffer[i] = SPI.transfer(0x00);
    }

    digitalWrite(FLASH_CS_PIN, HIGH);
    return true;
}

// funcao para transmitir os dados via rádio
void transmit_data(uint8_t* data, uint16_t length) {
    rf95.send(data, length);
    rf95.waitPacketSent();
    delay(1000); // 1 seg delay
}

void setup() {
    SPI.begin();
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH);
    initRadio();
}

void loop() {
    // transmitir dados GPS, total 50 bytes
    if (read_data_from_flash(0x000000, 50)) {
        transmit_data(flash_buffer, 50);
    }

    // transmitir dados barometro, total 8 bytes
    if (read_data_from_flash(0x2A00, 8)) {
        transmit_data(flash_buffer, 8);
    }

    // transmissao de outros dados (ainda nao definidos)
    flash_read_address = 0x3200;
    while (flash_read_address < 0xFFFFF) {
        if (read_data_from_flash(flash_read_address, FLASH_PAGE_SIZE)) {
            transmit_data(flash_buffer, FLASH_PAGE_SIZE);
        } else {
            break; // sai do loop, endereco estiver fora dos limites
        }
        flash_read_address += FLASH_PAGE_SIZE;
    }

    // reinicia o endereco
    flash_read_address = 0x000000;
}
