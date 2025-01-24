#ifndef SENSORS_DEFINITION_H
#define SENSORS_DEFINITION_H

//definicoes da memoria FLASH

#define FLASH_TOTAL_SIZE    0x80000000  // 2 GB
#define FLASH_PAGE_SIZE     256  // tamanho de uma página de memória na SPI Flash
#define GPS_INITIAL_ADDRESS  0x000000 //endereco inicial na memoria flash para salvar dados gps
#define BMP_INITIAL_ADDRESS  0x2A00 //endereco inicial na memoria flash para salvar dados barometro
#define UMI_INITIAL_ADDRESS  0x3200 //endereco inicial na memoria flash para salvar dados UMI
#define MCP_INITIAL_ADDRESS  0x4800 //endereco inicial na memoria flash para salvar dados do MCP
#define RTC_INITIAL_ADDRESS  
//definicoes dos pinos

#define FLASH_CS_PIN        5 // pino cs da memoria
#define RADIO_CS_PIN        14 // pino cs do radio
#define IMU_CS_PIN          12 // pino cs do imu

#define UART_TX_PIN         20 // Pino TX 
#define UART_RX_PIN         8 // Pino RX

#define I2C_SDA_PIN         15 // Pino SDA para I2C
#define I2C_SCL_PIN         27 // Pino SCL para I2C

#define SPI_CLK_PIN         19 // Pino CLK para SPI
#define SPI_MISO_PIN        22 // Pino MISO para SPI
#define SPI_MOSI_PIN        21 // Pino MOSI para SPI

#define RADIO_RST_PIN       7 // Pino RST do Radio
#define IMU_INT_PIN         33 // Pino INT do IMU
#define IMU_RST_PIN         32 // Pino RST do IMU


//definicoes dos enderecos de memoria

#define BMP_I2C_ADDRESS 0x76 //endereco de memoria do barometro
#define MCP_I2C_ADDRESS 0x18 //endereco de memoria do sensor de temperatura
#define RTC_I2C_ADDRESS 0x68 //endereco de memoria do RTC

#endif // SENSORS_DEFINITION_H