#include "Pines.h"

void inicializarMotores(){
  pinMode(BIN2  , OUTPUT);
  pinMode(BIN1  , OUTPUT);
  ledcSetup(1, freq, resolution);
  ledcAttachPin(PWMB, 1);
  pinMode(AIN1  , OUTPUT);
  pinMode(AIN2  , OUTPUT);
  ledcSetup(0, freq, resolution);
  ledcAttachPin(PWMA, 0);
}

void Motoriz(int value) {
  if ( value >= 0 ) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    value *= -1;
  }
  ledcWrite(0, value);
}
// Función accionamiento motor derecho
void Motorde(int value) {
  if ( value >= 0 ) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  } else {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    value *= -1;
  }
  ledcWrite(1, value);
}

//Accionamiento de motores
void Motor(int righ, int left) {
  Motoriz(left);
  Motorde(righ);
}

void girar(int direccion) {
  if (direccion == 1) {
    Motor(0, 0);
    delay(200);
    Motor(-80, 80);  // giro der
    delay(700);
  } else {
    Motor(0, 0);
    delay(200);
    Motor(80, -80);  // giro izq
    delay(700);
  }
}