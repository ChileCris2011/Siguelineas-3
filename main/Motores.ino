void inicializarMotores() {
  pinMode(BIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  ledcSetup(0, freq, resolution);
  ledcAttachPin(PWMB, 0);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  ledcSetup(1, freq, resolution);
  ledcAttachPin(PWMA, 1);
}

void MotorIzquierdo(int velocidad) {
  if (velocidad >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    velocidad *= -1;
  }
  ledcWrite(0, velocidad);
}

void MotorDerecho(int velocidad) {
  if (velocidad >= 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    velocidad *= -1;
  }
  ledcWrite(1, velocidad);
}

// Recibe velocidades con signo: positivo avanza, negativo retrocede.
void Motor(int velocidadIzquierda, int velocidadDerecha) {
  MotorIzquierdo(velocidadIzquierda);
  MotorDerecho(velocidadDerecha);
}
