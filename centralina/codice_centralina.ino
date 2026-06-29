#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

unsigned long startTime = millis();
bool success = false;
String mode = "Irrigatore"; // Modalità iniziale di default
volatile bool displayState = false;

// NRF24L01 GPIO e address di comunicazione
#define RADIO_MISO_PIN 12
#define RADIO_MOSI_PIN 13
#define RADIO_SCK_PIN 14
#define RADIO_CSN_PIN 15
#define RADIO_CE_PIN 16
const byte addressC[6] = "00001"; //ind centralina
const byte addressT[6] = "00002"; //ind broadcast
RF24 radio(RADIO_CE_PIN, RADIO_CSN_PIN);

// GPIO per i bottoni direzionali

#define BUTTON_N_PIN 26
#define BUTTON_S_PIN 27
#define BUTTON_W_PIN 23
#define BUTTON_E_PIN 25

volatile int buttonStateN = 0;
volatile int buttonStateS = 0;
volatile int buttonStateW = 0;
volatile int buttonStateE = 0;

//settaggio display OLED

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

void setup() {

  // Inizializzazione seriale e display
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  //settaggio pin dei bottoni e degli interrupt
  pinMode(BUTTON_N_PIN, INPUT);
  pinMode(BUTTON_S_PIN, INPUT);
  pinMode(BUTTON_W_PIN, INPUT);
  pinMode(BUTTON_E_PIN, INPUT);
  attachInterrupt(BUTTON_N_PIN, N_push, RISING);
  attachInterrupt(BUTTON_S_PIN, S_push, RISING);
  attachInterrupt(BUTTON_W_PIN, W_push, RISING);
  attachInterrupt(BUTTON_E_PIN, E_push, RISING);

  //apertura dei canali radio
  SPI.begin(RADIO_SCK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN, RADIO_CSN_PIN);
  radio.begin();
  radio.setAutoAck(false);
  radio.openWritingPipe(addressT);
  radio.openReadingPipe(1, addressC);
  radio.stopListening();

}

void ARDUINO_ISR_ATTR N_push() { // Interruttore per cambiare modalità
  if(mode == "Irrigatore"){
    mode = "Tapparella";
    displayState = true;
  }
  else if(mode == "Tapparella"){
    mode = "Irrigatore";
    displayState = true;
  }
}
void ARDUINO_ISR_ATTR S_push() {
  buttonStateS = 1;
}
void ARDUINO_ISR_ATTR W_push() {
  buttonStateW = 1;
}
void ARDUINO_ISR_ATTR E_push() {
  buttonStateE = 1;
}

void loop() {
  if (mode == "Irrigatore"){
    if (displayState){
      display.clearDisplay();
      displayState = false;
    }
    display.setCursor(0, 6);
    display.print("mod Irrigatore");
    display.display();
    if(buttonStateW == 1){
      //operazione di start irrigatore, invio radio e modifica display
      char data[] = "IRR START";
      radio.write(&data, sizeof(data));
      buttonStateW = 0;
      //una volta inviato il comando, il ricevitore aspetta un ack per 300ms
      radio.startListening();
      startTime = millis();
      while (millis() - startTime < 300) { // Attendere un breve periodo per ricevere l'ack
        if (radio.available()) {
          char ackData[32] = "";
          radio.read(&ackData, sizeof(ackData));
          if (String(ackData) == "ACK IRR START") {
            display.clearDisplay();
            display.setCursor(0, 16);
            display.print("Irrigatore avviato");
            display.display();
            success = true;
            break;
          }
        }
    }
    // Se non riceve l'ack entro il tempo stabilito, mostra un messaggio di errore
    radio.stopListening();
    if (!success) {
      display.clearDisplay();
      display.setCursor(0, 16);
      display.print("Errore avvio irr");
      display.display();
    }
    success = false; // Reset per prossima operazione
  }
    else if(buttonStateE == 1){
      char data[] = "IRR STOP";
      radio.write(&data, sizeof(data));
      buttonStateE = 0;
      radio.startListening();
      startTime = millis();
      while (millis() - startTime < 300) { // Attendere un breve periodo per ricevere l'ack
        if (radio.available()) {
          char ackData[32] = "";
          radio.read(&ackData, sizeof(ackData));
          if (String(ackData) == "ACK IRR STOP") {
            display.clearDisplay();
            display.setCursor(0, 16);
            display.print("Irrigatore fermato");
            display.display();
            success = true;
            break;
          }
        }
      }
      radio.stopListening();
      if (!success) {
        display.clearDisplay();
        display.setCursor(0, 16);
        display.print("Errore stop irr");
        display.display();
      }
      success = false; // Reset per prossima operazione
    }
  }

  if (mode == "Tapparella"){
    if (displayState){
      display.clearDisplay();
      displayState = false;
    }
    display.setCursor(0, 6);
    display.print("mod Tapparella");
    display.display();
    if(buttonStateW == 1){
      char data[] = "TAP UP";
      buttonStateW = 0;
      while (digitalRead(BUTTON_W_PIN) == HIGH) {
        radio.write(&data, sizeof(data));
        delay(50); //20 comandi al secondo
  }
    }
    else if(buttonStateE == 1){
      char data[] = "TAP DOWN";
      buttonStateE = 0;
      while (digitalRead(BUTTON_E_PIN) == HIGH) {
        radio.write(&data, sizeof(data));
        delay(50);
  }
    }
    /*
    else if(buttonStateS == 1){
      char data[] = "TAP TOGGLE";
      radio.write(&data, sizeof(data));
      buttonStateS = 0;
      radio.startListening();
      startTime = millis();
      while (millis() - startTime < 300) { // Attendere un breve periodo per ricevere l'ack
        if (radio.available()) {
          char ackData[32] = "";
          radio.read(&ackData, sizeof(ackData));
          if (String(ackData) == "ACK TAP TOGGLE") {
            display.clearDisplay();
            display.setCursor(0, 16);
            display.print("Stato tapp. cambiata");
            display.display();
            success = true;
            break;
          }
        }
      }
      radio.stopListening();
      if (!success) {
        display.clearDisplay();
        display.setCursor(0, 16);
        display.print("Errore cambio tap");
        display.display();
      }
      success = false; // Reset per prossima operazione
    }*/
  }
}
