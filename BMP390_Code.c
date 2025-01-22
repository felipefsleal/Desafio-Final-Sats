#include <stdint.h>
#include <stdio.h>
#include <Wire.h> //biblioteca para comunicacao I2C
#include <Adafruit_BMP3XX.h>  //biblioteca do barometro escolhido (BMP390)

#define BMP_ADRESS 0x76 //endereco de memoria do barometro
#define FLASH_PAGE_SIZE 256  // tamanho de uma página de memória na SPI Flash
#define SD_CS_PIN 5    // pino cs da memoria
#define I2C_SDA_PIN 15 // Pino SDA do I2C
#define I2C_SCL_PIN 27 // Pino SCL do I2C

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

    // copia os dados para o buffer

    memcpy(&flash_buffer[flash_buffer_index], &data, sizeof(Measurement));

    flash_buffer_index += sizeof(Measurement);

    // se o buffer estiver cheio, escreve no cartao SD
    if (flash_buffer_index >= FLASH_PAGE_SIZE) {

        File dataFile = SD.open("data.log", FILE_WRITE);
        
        if (dataFile) {
            
            dataFile.write(flash_buffer, FLASH_PAGE_SIZE);
            
            dataFile.close();
        }

        flash_write_address += FLASH_PAGE_SIZE;
        
        flash_buffer_index = 0; // reiniciar indice do buffer
    }
}

void setup() {
    
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
