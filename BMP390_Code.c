#include <stdint.h>
#include <stdio.h>
#include <Wire.h> //biblioteca para comunicacao I2C
#include <Adafruit_BMP3XX.h>  //biblioteca do barometro escolhido (BMP390)
#include <SPI.h>
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
uint32_t  flash_write_address = 0x000000; // endereco inicial na memória Flash

// funcao inicializar os componentes

void
system_init() {

    // inicializa o cartao SD

    if (!SD.begin(SD_CS_PIN)) {

        //erro ao inicializar, loop infinito evitando a continuacao do programa

        while (1);
    }
    
    // inicializa o BMP390
    
    if (!bmp.begin_SPI(BMP_CS_PIN)) {
      
         //erro ao inicializar, loop infinito evitando a continuacao do programa

        while (1);
    }

    bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
    bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_3);
    bmp.setOutputDataRate(BMP3_ODR_1_HZ);
}

// funcao para armazenar dados na SPI Flash

void
store_data_in_flash(float pressure, float temperature) {
    Measurement data;
    data.pressure = pressure;
    data.temperature = temperature;

    // copia dados buffer
    memcpy(&flash_buffer[flash_buffer_index], &data, sizeof(Measurement));
    flash_buffer_index += sizeof(Measurement);

    // se o buffer estiver cheio, escreve na memoria flash via SPI
    if (flash_buffer_index >= FLASH_PAGE_SIZE) {
        digitalWrite(SD_CS_PIN, LOW); // seleciona chip memoria flash

        // escrever endereco na memória flash
        SPI.transfer(0x02); // Comando de gravação (WRITE)
        SPI.transfer((flash_write_address >> 16) & 0xFF); // Endereço MSB
        SPI.transfer((flash_write_address >> 8) & 0xFF); // Endereço meio
        SPI.transfer(flash_write_address & 0xFF); // Endereço LSB

        // transfere dados buffer para memoria flash
        for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++) {
            SPI.transfer(flash_buffer[i]);
        }

        digitalWrite(SD_CS_PIN, HIGH);

        // atualiza endereco de gravacao, reinicia indice buffer
        flash_write_address += FLASH_PAGE_SIZE;
        flash_buffer_index = 0;
    }
}

void
setup() {
    
    Serial.begin(115200); // inicializa a comunicação serial 
    
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // inicializa os pinos I2C
    
    system_init();
}


int
main() {

    system_init();

    while (1) {
        float pressure = 0.0, temperature = 0.0;

        // dados do BMP390
        if (bmp.performReading()) {
            
            pressure = bmp.pressure / 100.0; // converte para hPa
            
            temperature = bmp.temperature; // temperatura em C

            // armeznar dados na memória SPI Flash
            
            store_data_in_flash(pressure, temperature);

        }

        // Aguarda 1 segundo antes da próxima leitura
        
        delay(1000);
    }

    return 0;
}
