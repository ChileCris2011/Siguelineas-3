void girarDerecha(float grados) {
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

void girarIzquierda(float grados) {
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

void girar(int direccion) {
  if (direccion == 1) {
    girarDerecha(85);
  } else {
    girarIzquierda(85);
  }
}

void plusgirar(int direccion) {
  if (direccion == 1) {
    girarDerecha(95);
  } else {
    girarIzquierda(95);
  }
}

void girarCrudo(int direccion) {
  if (direccion == 1) {
    Motor(baseGiros, -baseGiros);  // giro der
  } else {
    Motor(-baseGiros, baseGiros);  // giro izq
  }
}

void giroWhile(int direccion) {
  qtr.read(sensorValues);
  while (sensorValues[((direccion == 1) ? 3 : 4)] < 3500) {
    qtr.read(sensorValues);
    girarCrudo(direccion);
  }
}