#ifndef SENSORS_DEFINITION_H
#define SENSORS_DEFINITION_H

//definicoes da memoria FLASH

#define FLASH_TOTAL_SIZE 0x80000000  // 2 GB
#define FLASH_PAGE_SIZE 256  // tamanho de uma página de memória na SPI Flash
#define BMP_INITIAL_ADRESS 0x000000 // endereco inicial na memoria do barometro BMP


//definicoes dos pinos

#define SD_CS_PIN 5    // pino cs da memoria
#define I2C_SDA_PIN 15 // Pino SDA do I2C
#define I2C_SCL_PIN 27 // Pino SCL do I2C


//definicoes dos enderecos de memoria

#define BMP_ADRESS 0x76 //endereco de memoria do barometro

#endif // SENSORS_DEFINITION_H