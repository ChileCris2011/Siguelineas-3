void Girar(float grados, int direction) {
  mpu.update();
  float anguloInicio = mpu.getAngleZ();
  float anguloActual = anguloInicio;

  if (direction == Derecha) {
    while ((anguloActual - anguloInicio) > -grados) {
      Motor(velBaseIzq, -velBaseDer);  // izquierda
      mpu.update();
      anguloActual = mpu.getAngleZ();
    }
  } else if (direction == Izquierda) {
    while ((anguloActual - anguloInicio) < grados) {
      Motor(-velBaseIzq, velBaseDer);  // derecha
      mpu.update();
      anguloActual = mpu.getAngleZ();
    }
  }
}

void girarCrudo(int direccion) {
  if (direccion == Derecha) {
    Motor(velBaseIzq, -velBaseDer);  // giro der
  } else {
    Motor(-velBaseDer, velBaseIzq);  // giro izq
  }
}

void giroWhile(int direccion, bool ver) {
  qtr.read(sensorValues);
  if (ver) { // Salir de la linea antes de girar. Evita parar en la misma línea de la que salió. false por defecto
    while (sensorValues[((direccion == Derecha) ? 3 : 4)] > umbral) {
      qtr.read(sensorValues);
      girarCrudo(direccion);
    }
    Motor(0, 0);
    delay(100); // No mas para ver si se está usando.
  }
  while (sensorValues[((direccion == Derecha) ? 3 : 4)] < umbral) {
    qtr.read(sensorValues);
    girarCrudo(direccion);
  }
}