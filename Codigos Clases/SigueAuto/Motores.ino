// Función inicialización de puente h

void inicializarMotores() {
  pinMode(BIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  ledcSetup(1, frecuencia, resolution);
  ledcAttachPin(PWMB, 1);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  ledcSetup(0, frecuencia, resolution);
  ledcAttachPin(PWMA, 0);
}

// Función accionamiento motor izquierdo
void Motoriz(int value) {
  if (value >= 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } else {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    value *= -1;
  }
  ledcWrite(1, value);
}

// Función accionamiento motor derecho
void Motorde(int value) {
  if (value >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    value *= -1;
  }
  ledcWrite(0, value);
}

// Accionamiento de motores
void Motor(int left, int righ) {
  Motoriz(left);
  Motorde(righ);
}

// Función para girar el robot
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