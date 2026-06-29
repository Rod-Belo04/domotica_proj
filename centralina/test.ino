#include <AccelStepper.h>

// Demo sketch per TB6600 + NEMA17.
// Il TB6600 usa segnali STEP/PUL e DIR, quindi AccelStepper va usata
// in modalita DRIVER: un pin genera gli impulsi, l'altro imposta la direzione.

// Collegamenti dal file connections.md, sezione "Controllo Tapparelle".
#define STEPPER_PUL_PIN 17
#define STEPPER_DIR_PIN 18

// Valori di test: da regolare in base a microstepping, meccanica e corrente.
#define LEFT_TARGET_STEPS -1600
#define RIGHT_TARGET_STEPS 1600
#define LEFT_SPEED_STEPS_S 400
#define RIGHT_SPEED_STEPS_S 900
#define ACCELERATION_STEPS_S2 800
#define PAUSE_BETWEEN_MOVES_MS 1000

// DRIVER = interfaccia step/dir, adatta a driver come TB6600.
AccelStepper stepper(AccelStepper::DRIVER, STEPPER_PUL_PIN, STEPPER_DIR_PIN);

enum MoveState {
  MOVE_LEFT,
  PAUSE_AFTER_LEFT,
  MOVE_RIGHT,
  PAUSE_AFTER_RIGHT
};

MoveState moveState = MOVE_LEFT;
unsigned long pauseStartTime = 0;

void setup() {
  Serial.begin(9600);

  stepper.setAcceleration(ACCELERATION_STEPS_S2);

  // Primo movimento: verso sinistra, piu lento.
  stepper.setMaxSpeed(LEFT_SPEED_STEPS_S);
  stepper.moveTo(LEFT_TARGET_STEPS);
}

void loop() {
  // run() non blocca: genera gli step necessari e poi ritorna subito.
  // Va chiamata il piu spesso possibile.
  stepper.run();

  switch (moveState) {
    case MOVE_LEFT:
      if (stepper.distanceToGo() == 0) {
        pauseStartTime = millis();
        moveState = PAUSE_AFTER_LEFT;
      }
      break;

    case PAUSE_AFTER_LEFT:
      if (millis() - pauseStartTime >= PAUSE_BETWEEN_MOVES_MS) {
        // Secondo movimento: verso destra, piu veloce.
        stepper.setMaxSpeed(RIGHT_SPEED_STEPS_S);
        stepper.moveTo(RIGHT_TARGET_STEPS);
        moveState = MOVE_RIGHT;
      }
      break;

    case MOVE_RIGHT:
      if (stepper.distanceToGo() == 0) {
        pauseStartTime = millis();
        moveState = PAUSE_AFTER_RIGHT;
      }
      break;

    case PAUSE_AFTER_RIGHT:
      if (millis() - pauseStartTime >= PAUSE_BETWEEN_MOVES_MS) {
        // Torna a sinistra a velocita ridotta.
        stepper.setMaxSpeed(LEFT_SPEED_STEPS_S);
        stepper.moveTo(LEFT_TARGET_STEPS);
        moveState = MOVE_LEFT;
      }
      break;
  }
}
