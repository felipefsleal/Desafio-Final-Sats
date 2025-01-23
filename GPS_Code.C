#include <stdint.h>
#include <stdio.h>
#include <Adafruit_GPS.h> // biblioteca para o modulo GPS
#include <SPI.h>
#include "sensors_definition.h"

#define GPSSerial Serial1 // porta serial GPS

// dados do GPS

typedef struct {

    char latitude[15];
    char longitude[15];
    char date[11];
    char time[9];

} GPSData;

// objeto GPS

Adafruit_GPS GPS(&GPSSerial);

// buffer para armazenamento memoria flash

uint8_t flash_buffer[FLASH_PAGE_SIZE];
uint16_t flash_buffer_index = 0;
uint32_t flash_write_address = 0x1000; // endereco inicial memoria flash (alterar depois)

// funcao inicializar o GPS
void
initGPS() {

    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    GPS.sendCommand(PGCMD_ANTENNA);
    delay(1000);
    GPSSerial.println(PMTK_Q_RELEASE);
}

// funcao para coletar dados

GPSData
getGPSData(){

    GPSData data;

    memset(&data, 0, sizeof(data)); // incializar a estruutura com 0
    
    if (GPS.newNMEAreceived()) {
        
        if (!GPS.parse(GPS.lastNMEA())) {
            
            return data; // retorna dados vazios se a sentenca e invalida
        }
    }

    if (GPS.fix) {
        
        snprintf(data.latitude, sizeof(data.latitude), "%f %c", GPS.latitude, GPS.lat);
        snprintf(data.longitude, sizeof(data.longitude), "%f %c", GPS.longitude, GPS.lon);
        snprintf(data.date, sizeof(data.date), "20%02d-%02d-%02d", GPS.year, GPS.month, GPS.day);
        snprintf(data.time, sizeof(data.time), "%02d:%02d:%02d", GPS.hour, GPS.minute, GPS.seconds);
    }

    return data;
}

// funcao para armazenar dados na memoria flash

void
store_data_in_flash(GPSData *data) {
    
    // Copia os dados para o buffer
    memcpy(&flash_buffer[flash_buffer_index], data, sizeof(GPSData));
    
    flash_buffer_index += sizeof(GPSData);

    // Se o buffer estiver cheio, grava na memoria flash
    if (flash_buffer_index >= FLASH_PAGE_SIZE) {
        
        // selecionar o chip de memoria flash
        digitalWrite(SD_CS_PIN, LOW);

        // envia o comando de escrita e endereço para a memoria flash
        SPI.transfer(0x02); // comando de gravação 
        SPI.transfer((flash_write_address >> 16) & 0xFF); // endereco MSB
        SPI.transfer((flash_write_address >> 8) & 0xFF);
        SPI.transfer(flash_write_address & 0xFF); // endereco LSB
 
        // transfere os dados do buffer para a memoria flash
        for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++) {
            SPI.transfer(flash_buffer[i]);
        }

        digitalWrite(SD_CS_PIN, HIGH);

        flash_buffer_index = 0; // reinicia o buffer

        // Atualiza o endereço de escrita
        flash_write_address += FLASH_PAGE_SIZE;
        flash_buffer_index = 0; // reinicia o índice do buffer
    }
}

void
setup() {
    
    // inicializa a comunicacao SPI
    SPI.begin();
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    // inicializa o GPS
    initGPS();
}

void
loop() {

    // coleta os dados do GPS
    GPSData data = getGPSData();

    // armazena os dados na memoria flash
    storeDataInFlash(&data);

    // aguarda 1 segundo para reiniciar os dados
    delay(1000);

}
