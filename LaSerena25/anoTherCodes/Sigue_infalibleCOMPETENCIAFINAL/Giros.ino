void girarDerecha(float grados) {
  mpu.update();
  float anguloInicio = mpu.getAngleZ();
  float anguloActual = anguloInicio;
  Motor(50, 50);
  delay(5);
  while ((anguloActual - anguloInicio) > -grados) {
    Motor(giroBase, -giroBase);  // izquierda
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
  delay(5);
  while ((anguloActual - anguloInicio) < grados) {
    Motor(-giroBase, giroBase);  // derecha
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

void girarCrudo(int direccion) {
  if (direccion == 1) {
    Motor(giroBase, -giroBase);  // giro der
  } else {
    Motor(-giroBase, giroBase);  // giro izq
  }
}
