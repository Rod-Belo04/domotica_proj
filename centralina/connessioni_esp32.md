# Collegamenti Rete Smart Home

Questo file documenta i collegamenti hardware per ogni dispositivo basato su ESP32 nella rete smart home.

## Centralina

Unita di controllo centralizzata basata su ESP32-WROOM-32U. Utilizza un modulo radio NRF24L01, quattro pulsanti e un display OLED SSD1306.

| GPIO ESP32 | Dispositivo | Pin / Segnale Dispositivo | Direzione |
| --- | --- | --- | --- |
| 12 | NRF24L01 | MISO | OUTPUT |
| 13 | NRF24L01 | MOSI | INPUT |
| 14 | NRF24L01 | SCK | OUTPUT |
| 15 | NRF24L01 | CSN | OUTPUT |
| 16 | NRF24L01 | CE | OUTPUT |
| 21 | SSD1306 OLED | SDA | NA |
| 22 | SSD1306 OLED | SCL | NA |
| 3.3V | SSD1306 OLED | VCC | OUTPUT |
| 26 | Pulsante N | 26 | INPUT |
| 27 | Pulsante S | 27 | INPUT |
| 23 | Pulsante W | 23 | INPUT |
| 25 | Pulsante E | 25 | INPUT |

## Irrigatore

Nodo di controllo dell'irrigatore basato su ESP32. Riceve comandi tramite un modulo radio NRF24L01 e controlla una pompa dell'acqua tramite un rele. La pompa e collegata al lato alta tensione/carico del rele, non direttamente all'ESP32.

| GPIO ESP32 | Dispositivo | Pin / Segnale Dispositivo | Direzione |
| --- | --- | --- | --- |
| 12 | NRF24L01 | MISO | OUTPUT |
| 13 | NRF24L01 | MOSI | INPUT |
| 14 | NRF24L01 | SCK | OUTPUT |
| 15 | NRF24L01 | CSN | OUTPUT |
| 16 | NRF24L01 | CE | OUTPUT |
| 26 | Modulo Rele | IN / Segnale | OUTPUT |
| 5V | Modulo Rele | VCC | OUTPUT |
| GND | Modulo Rele | GND | NA |

## Controllo Tapparelle

Nodo di controllo tapparelle basato su ESP32. Riceve comandi tramite un modulo radio NRF24L01 e pilota un motore passo-passo tramite un driver per stepper motor.

| GPIO ESP32 | Dispositivo | Pin / Segnale Dispositivo | Direzione |
| --- | --- | --- | --- |
| 12 | NRF24L01 | MISO | OUTPUT |
| 13 | NRF24L01 | MOSI | INPUT |
| 14 | NRF24L01 | SCK | OUTPUT |
| 26 | NRF24L01 | CSN | OUTPUT |
| 4 | NRF24L01 | CE | OUTPUT |
| 17 | Driver Stepper | PUL+ | OUTPUT |
| 18 | Driver Stepper | DIR+ | OUTPUT |
| 24V | Driver Stepper | VCC HV | OUTPUT |
| GND | Driver Stepper | GND | NA |
