#include "Pines.h"

void inicializarMotores() {
  pinMode(BIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  ledcSetup(1, freq, resolution);
  ledcAttachPin(PWMB, 1);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  ledcSetup(0, freq, resolution);
  ledcAttachPin(PWMA, 0);
}

// Función para el motor izquierdo
void Motoriz(int valueiz) {
  if (valueiz >= 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } else {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    valueiz = -valueiz;
  }
  ledcWrite(1, valueiz); // ← CAMBIADO de 0 a 1
}

// Función para el motor derecho
void Motorde(int valueder) {
  if (valueder >= 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  } else {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    valueder = -valueder;
  }
  ledcWrite(0, valueder); // ← CAMBIADO de 1 a 0
}


//Accionamiento de motores
void Motor(int left, int righ) {
  Motoriz(righ);
  Motorde(left);
}
