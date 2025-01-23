#include <stdint.h>
#include <SPI.h>
#include <RH_RF95.h>
#include "sensors_definition.h"


#define RADIO_FREQ 915.0   // Frequência do rádio em MHz

// buffer para leitura da memoria Flash
uint8_t flash_buffer[FLASH_PAGE_SIZE];
uint32_t flash_read_address = 0x1000; // endereco inicial da memoria Flash

// objeto radio
RH_RF95 rf95(RADIO_CS_PIN, RFM95_INT_PIN);

// funcao para inicializar o radio

void
initRadio() {
    // configura o radio
    if (!rf95.init()) {
        while (1); // trava o programa se o radio nao inicializar
    }
    rf95.setFrequency(RADIO_FREQ);  // configura a frequencia
    rf95.setTxPower(23, false);     // configura a potencia de transmissao
}

// funcao para ler dados da memoria Flash
bool
read_data_from_flash() {
    memset(flash_buffer, 0, FLASH_PAGE_SIZE); // reseta o buffer

    digitalWrite(FLASH_CS_PIN, LOW);

    // envia o comando de leitura e o endereco para a memoria Flash
    SPI.transfer(0x03); // comando de leitura
    SPI.transfer((flash_read_address >> 16) & 0xFF); // endereco MSB
    SPI.transfer((flash_read_address >> 8) & 0xFF);  // endereco medio
    SPI.transfer(flash_read_address & 0xFF);         // endereco LSB

    // le dados da memoria para o buffer
    for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++) {
        flash_buffer[i] = SPI.transfer(0x00);
    }

    digitalWrite(FLASH_CS_PIN, HIGH);

    // atualiza o endereco de leitura
    flash_read_address += FLASH_PAGE_SIZE;

    // retorna falso se o endereco de leitura ultrapassar o limite
    return flash_read_address < 0xFFFFF; // ajuste conforme o tamanho da sua memoria
}

// funcao para transmitir os dados via radio

void
transmit_data() {

    rf95.waitPacketSent(); // aguarda ate a transmissao ser concluida
    delay(1000);           // aguarda 1 segundo entre transmissoes
}


void
setup() {
    // inicializa SPI, memoria Flash e radio
    SPI.begin();
    pinMode(FLASH_CS_PIN, OUTPUT);
    pinMode(RADIO_RST_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH);

    // configura o radio
    initRadio();
}

void
loop() {
    // le os dados da memoria Flash
    if (read_data_from_flash()) {
        // Transmite os dados lidos
        transmit_data();
    } else {
        // reinicia o endereco de leitura caso alcance fim memoria
        flash_read_address = 0x1000;
    }
}
