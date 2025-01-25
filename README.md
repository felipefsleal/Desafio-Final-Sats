# Códigos Embarcados Satélite - Dionísio

Este repositório contém os códigos desenvolvidos para o controle de sensores e do sistema operacional de um satélite, criado pelos trainees do **Grupo de Pesquisas Aeroespaciais da Universidade Federal do Rio de Janeiro (UFRJ)**. O projeto tem como objetivo principal integrar diferentes sensores em um sistema embarcado para coleta e transmissão de dados durante a operação do satélite.

## Códigos do Repositório

Os arquivos estão organizados por funcionalidade e tipo de sensor:

- **`BMP390_Code.c`**: Código para o sensor de pressão BMP390, utilizado para medições de pressão e temperatura.
- **`GPS_Code.C`**: Implementação para o módulo GPS, responsável pela aquisição de dados de localização.
- **`INA219.c`**: Código para o sensor de corrente INA219, usado para monitoramento da corrente e voltagem do sistema.
- **`MCP9808.c`**: Código de para o sensor de temperatura, utilizado para monitorar a placa de EPS.

- **`RADIO_Code.c`**: Funções para transmissão de dados via módulo de comunicação por rádio.
- **`RTC.c`**: Implementação do módulo de tempo real (RTC) para sincronização temporal dos dados.
- **`UMI.c`**: Código para o sensor de unidade de medida inercial, coletando os seguintes dados: Aceleração, velocidade angular, campo magnético.
- **`sensors_definition.h`**: Arquivo de cabeçalho contendo definições globais, como pinos e configurações dos sensores.
- **`espectrofotometro.ino`**: Código para espectrofotômetro, desenvolvido pela equipe de payload para medir a absorção de luz.

## Tecnologias Utilizadas

- Linguagem de programação: **C**
- Protocolo de comunicação: **I2C**, **SPI**, **UART**
- Microcontrolador que será embarcado: **ESP 32**


---

## Colaboradores

- **Otávio Braga** ( otaviob.moraes.20241@poli.ufrj.br )
- **Felipe Leal** ( felipe.leal.20241@poli.ufrj.br )
- **Lucas Pacheco** ( lucasmpacheco.20241@poli.ufrj.br )
- **Julia Acsa** ( juliamila.20242@poli.ufrj.br )
