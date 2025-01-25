#include <stdint.h>
#include <SPI.h>
#include <Adafruit_BNO08x.h> // biblioteca sensor BNO08x


typedef struct {
    float orientation[3]; // orientacao em graus
    float acceleration[3]; // aceleracao m/s2
} Measurement;

// objeto sensor BNO08x
Adafruit_BNO08x bno;

// buffer para a memoria SPI flash
uint8_t flash_buffer[FLASH_PAGE_SIZE];
uint16_t flash_buffer_index = 0;
uint32_t flash_write_address = UMI_INITIAL_ADDRESS;

// funcao para inicializar componentes
void system_init() {
    // inicializa o BNO08x utilizando SPI
    if (!bno.begin_SPI(FLASH_CS_PIN, IMU_INT_PIN)) {
        Serial.println("Erro ao inicializar o BNO08x via SPI.");
        while (1);
    }

    // configura BNO08x
    if (!bno.enableReport(SH2_GAME_ROTATION_VECTOR)) {
        Serial.println("Erro ao habilitar relatório de orientação!");
    }
    if (!bno.enableReport(SH2_ACCELEROMETER)) {
        Serial.println("Erro ao habilitar relatório de aceleração!");
    }

    // inicializa SPI memoria Flash
    SPI.begin();
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); 
}

// funcao armazenar dados na SPI Flash
void store_data_in_flash(float orientation[3], float acceleration[3]) {
    Measurement data;
    for (int i = 0; i < 3; i++) {
        data.orientation[i] = orientation[i];
        data.acceleration[i] = acceleration[i];
    }

    // verifica buffer espaco suficiente
    if ((flash_buffer_index + sizeof(Measurement)) > FLASH_PAGE_SIZE) {
        Serial.println("Erro: Buffer cheio antes de gravar na SPI Flash!");
        return;
    }

    // copia dados para buffer
    memcpy(&flash_buffer[flash_buffer_index], &data, sizeof(Measurement));
    flash_buffer_index += sizeof(Measurement);

    // verifica buffer cheio e grava memoria
    if (flash_buffer_index >= FLASH_PAGE_SIZE) {
        digitalWrite(FLASH_CS_PIN, LOW); // seleciona o chip da memoria flash

        // envia o comando de gravacao
        SPI.transfer(0x02); 
        SPI.transfer((flash_write_address >> 16) & 0xFF); // endereco MSB
        SPI.transfer((flash_write_address >> 8) & 0xFF);  // endereco medio
        SPI.transfer(flash_write_address & 0xFF);         // endereco LSB

        // transfere dados buffer para memoria
        for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++) {
            SPI.transfer(flash_buffer[i]);
        }

        digitalWrite(FLASH_CS_PIN, HIGH); // desativa chip da memoria flash

        // atualiza endereco de gravacao, reinicia indice buffer
        flash_write_address += FLASH_PAGE_SIZE;
        flash_buffer_index = 0;
    }
}

void setup() {
    Serial.begin(115200);            // inicializa a comunicacao serial
    system_init();                   // inicializa os sensores e a memoria
}

void loop() {
    float orientation[3] = {0.0, 0.0, 0.0};
    float acceleration[3] = {0.0, 0.0, 0.0};

    // obtem dados do sensor
    sh2_SensorValue_t sensorValue;

    while (bno.getSensorEvent(&sensorValue)) { 
        switch (sensorValue.sensorId) {
            case SH2_GAME_ROTATION_VECTOR: // dados orientacao
                orientation[0] = sensorValue.un.gameRotationVector.i * 360.0; 
                orientation[1] = sensorValue.un.gameRotationVector.j * 360.0; 
                orientation[2] = sensorValue.un.gameRotationVector.k * 360.0; 
                break;

            case SH2_ACCELEROMETER: // dados de aceleracao
                acceleration[0] = sensorValue.un.accelerometer.x; // aceleracao X
                acceleration[1] = sensorValue.un.accelerometer.y; // aceleracao Y
                acceleration[2] = sensorValue.un.accelerometer.z; // aceleracao Z
                break;

            default:
                break;
        }
    }

    // armazena dados na memoria
    store_data_in_flash(orientation, acceleration);

    // delay de 1 seg
    delay(1000);
}
