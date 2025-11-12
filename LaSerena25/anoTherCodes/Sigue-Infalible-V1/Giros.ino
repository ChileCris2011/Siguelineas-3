void girarDerecha(float grados) {
  mpu.update();
  float anguloInicio = mpu.getAngleZ();
  float anguloActual = anguloInicio;
  Motor(50, 50);
  delay(5);
  while ((anguloActual - anguloInicio) > -grados) {
    Motor(50, -50);  // izquierda
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
    Motor(-50, 50);  // derecha
    mpu.update();
    anguloActual = mpu.getAngleZ();
  }
  Motor(50, 50);
  delay(200);
  Motor(0, 0);
}