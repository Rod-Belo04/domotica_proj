#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <AccelStepper.h>

// NRF24L01 GPIO e address di comunicazione
#define RADIO_MISO_PIN 12
#define RADIO_MOSI_PIN 13
#define RADIO_SCK_PIN 14
#define RADIO_CSN_PIN 26
#define RADIO_CE_PIN 4

// pin per driver stepper TB6600
#define STEPPER_PUL_PIN 17
#define STEPPER_DIR_PIN 18

//posizione dove la tapparella è chiusa, da regolare ai singoli casi
#define POS_CLOSE 2000

const byte addressC[6] = "00001"; // ind centralina
const byte addressT[6] = "00002"; // ind broadcast

int TargetPos = 0; // Posizione target della tapparella, parte da aperta

RF24 radio(RADIO_CE_PIN, RADIO_CSN_PIN);
AccelStepper stepper(AccelStepper::DRIVER, STEPPER_PUL_PIN, STEPPER_DIR_PIN);

void setup() {
  Serial.begin(9600);

  // apertura dei canali radio
  SPI.begin(RADIO_SCK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN, RADIO_CSN_PIN);
  radio.begin();
  radio.setAutoAck(false);
  radio.openWritingPipe(addressC);
  radio.openReadingPipe(1, addressT);
  radio.startListening();

  // Imposta la velocità massima e l'accelerazione dello stepper
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(2000);
}

void loop(){
    if (radio.available()){
        char data[32] = "";
        radio.read(&data, sizeof(data));
        if (String(data) == "TAP UP") {
            if (TargetPos > 0){
                TargetPos = TargetPos - 20;
                stepper.moveTo(TargetPos);
            }
        }
        else if (String(data) == "TAP DOWN") {
            if (TargetPos < POS_CLOSE){
                TargetPos = TargetPos + 20;
                stepper.moveTo(TargetPos);
            }
        }
        
    }
    stepper.run();
}

