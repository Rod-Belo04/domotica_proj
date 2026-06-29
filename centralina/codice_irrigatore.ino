#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// NRF24L01 GPIO e address di comunicazione
#define RADIO_MISO_PIN 12
#define RADIO_MOSI_PIN 13
#define RADIO_SCK_PIN 14
#define RADIO_CSN_PIN 15
#define RADIO_CE_PIN 16

#define RELAY_PIN 26
#define IRRIGAZIONE_TEMP 300000UL

bool Att_irr = false; // Variabile di stato dell'irrigatore
unsigned long irrigationStartTime = 0;

const byte addressC[6] = "00001"; // ind centralina
const byte addressT[6] = "00002"; // ind broadcast

RF24 radio(RADIO_CE_PIN, RADIO_CSN_PIN);

void setup() {
  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // apertura dei canali radio
  SPI.begin(RADIO_SCK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN, RADIO_CSN_PIN);
  radio.begin();
  radio.setAutoAck(false);
  radio.openWritingPipe(addressC);
  radio.openReadingPipe(1, addressT);
  radio.startListening();
}

void loop() {
    if (radio.available()) {
        char data[32] = "";
        radio.read(&data, sizeof(data));
        if (String(data) == "IRR START") {
            // Avvia l'irrigatore
            Serial.println("Irrigatore avviato");
            char ackData[] = "ACK IRR START";
            delay(100); //breve ritardo per lo "swap" tra ricezione e trasmissione della centralina
            radio.stopListening();
            radio.write(&ackData, sizeof(ackData));
            radio.startListening();
            Att_irr = true; // Imposta la variabile di stato dell'irrigatore su true
            irrigationStartTime = millis();
        } else if (String(data) == "IRR STOP") {
            // Ferma l'irrigatore
            char ackData[] = "ACK IRR STOP";
            Att_irr = false;
            digitalWrite(RELAY_PIN, LOW);
            delay(100); //breve ritardo per lo "swap" tra ricezione e trasmissione della centralina
            radio.stopListening();
            radio.write(&ackData, sizeof(ackData));
            radio.startListening();
        }
    }

    if (Att_irr) {
        digitalWrite(RELAY_PIN, HIGH);

        if (millis() - irrigationStartTime >= IRRIGAZIONE_TEMP) {
            Att_irr = false;
            digitalWrite(RELAY_PIN, LOW);
        }
    }
}
