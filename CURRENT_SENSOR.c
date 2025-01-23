#include <Wire.h>
#include <Adafruit_INA219.h>
#include <stdio.h>
#include <stdint.h>
#include <SPI.h> //biblioteca para comunicacao SPI


// Funções fictícias para simular a leitura de sensores
float getShuntVoltage_mV() {
    // Simular leitura
    return 75.0;
}

float getBusVoltage_V() {
    // Simular leitura
    return 12.0;
}

float getCurrent_mA() {
    // Simular leitura
    return 150.0;
}

float getPower_mW() {
    // Simular leitura
    return 1800.0;
}

int main() {
    printf("Hello!\n");

    // Inicializar o INA219 (simulado)
    int ina219_initialized = 1; // Simular que foi inicializado com sucesso

    if (!ina219_initialized) {
        printf("Failed to find INA219 chip\n");
        return 1;
    }

    printf("Measuring voltage and current with INA219 ...\n");

    while (1) {
        float shuntvoltage = 0;
        float busvoltage = 0;
        float current_mA = 0;
        float loadvoltage = 0;
        float power_mW = 0;

        shuntvoltage = getShuntVoltage_mV();
        busvoltage = getBusVoltage_V();
        current_mA = getCurrent_mA();
        power_mW = getPower_mW();
        loadvoltage = busvoltage + (shuntvoltage / 1000);

        printf("Bus Voltage:   %.2f V\n", busvoltage);
        printf("Shunt Voltage: %.2f mV\n", shuntvoltage);
        printf("Load Voltage:  %.2f V\n", loadvoltage);
        printf("Current:       %.2f mA\n", current_mA);
        printf("Power:         %.2f mW\n", power_mW);
        printf("\n");

        // Simular delay
        usleep(2000000); // 2 segundos
    }

    return 0;
}
