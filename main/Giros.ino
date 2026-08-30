int girarDerecha(float grados, bool detenerAlEncontrarLinea) {
  mpu.update();
  float anguloInicio = mpu.getAngleZ();
  float anguloActual = anguloInicio;
  while ((anguloActual - anguloInicio) > -grados) {
    Motor(baseGiros, -baseGiros);  // izquierda
    mpu.update();
    anguloActual = mpu.getAngleZ();
    if (detenerAlEncontrarLinea) {
      qtr.read(sensorValues);
      if (sensorValues[3] > umbral || sensorValues[4] > umbral) {
        Motor(0, 0);
        delay(200);
        return 1;
      }
    }
  }
  Motor(0, 0);
  delay(200);
  return 0;
}

int girarIzquierda(float grados, bool detenerAlEncontrarLinea) {
  mpu.update();
  float anguloInicio = mpu.getAngleZ();
  float anguloActual = anguloInicio;
  while ((anguloActual - anguloInicio) < grados) {
    Motor(-baseGiros, baseGiros);  // derecha
    mpu.update();
    anguloActual = mpu.getAngleZ();
    if (detenerAlEncontrarLinea) {
      qtr.read(sensorValues);
      if (sensorValues[3] > umbral || sensorValues[4] > umbral) {
        Motor(0, 0);
        delay(200);
        return 1;
      }
    }
  }
  Motor(0, 0);
  delay(200);
  return 0;
}

void girar(int direccion) {
  if (direccion == 1) {
    girarDerecha(87);
  } else {
    girarIzquierda(87);
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
  // Termina cuando el sensor central del lado opuesto vuelve a tomar la linea.
  while (sensorValues[((direccion == 1) ? 3 : 4)] < 3500) {
    qtr.read(sensorValues);
    girarCrudo(direccion);
  }
}

void giroSal(int direccion) {
  qtr.read(sensorValues);
  // Sale primero de la linea central para que el siguiente giro no se corte demasiado pronto.
  while (sensorValues[3] > 3500 || sensorValues[4] > 3500) {
    qtr.read(sensorValues);
    girarCrudo(direccion);
  }
}
