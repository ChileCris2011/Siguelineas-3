Motores_ Motores;

void Motores_::begin(void) {
  pinMode(BIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  ledcSetup(0, freq, resolution);
  ledcAttachPin(PWMB, 0);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  ledcSetup(1, freq, resolution);
  ledcAttachPin(PWMA, 1);
}

void Motores_::izq(int value) {
  if (value >= 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    value *= -1;
  }
  ledcWrite(0, value);
}

void Motores_::der(int value) {
  if (value >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    value *= -1;
  }
  ledcWrite(1, value);
}

void Motores_::drive(int left, int righ) {
  Motoriz(left);
  Motorde(righ);
}

void Motores_::girar.gyro(int direccion, float grados) {
  if (direccion == 1) {
    girarDerecha(grados);
  } else {
    girarIzquierda(grados);
  }
}

void Motores_::girar.undef(int direccion) {
  if (direccion == 1) {
    Motor(baseGiros, -baseGiros);  // giro der
  } else {
    Motor(-baseGiros, baseGiros);  // giro izq
  }
}

void Motores_::girar.while(int direccion) {
  qtr.read(sensorValues);
  while (sensorValues[((direccion == 1) ? 3 : 4)] < 3500) {
    qtr.read(sensorValues);
    girar_undef(direccion);
  }
}

void gDer(float grados) {
  mpu.update();
  float anguloInicio = mpu.getAngleZ();
  float anguloActual = anguloInicio;
  Motor(50, 50);
  delay(200);
  while ((anguloActual - anguloInicio) > -grados) {
    Motor(baseGiros, -baseGiros);  // izquierda
    mpu.update();
    anguloActual = mpu.getAngleZ();
  }
  Motor(50, 50);
  delay(200);
  Motor(0, 0);
}

void gIzq(float grados) {
  mpu.update();
  float anguloInicio = mpu.getAngleZ();
  float anguloActual = anguloInicio;
  Motor(50, 50);
  delay(200);
  while ((anguloActual - anguloInicio) < grados) {
    Motor(-baseGiros, baseGiros);  // derecha
    mpu.update();
    anguloActual = mpu.getAngleZ();
  }
  Motor(50, 50);
  delay(200);
  Motor(0, 0);
}