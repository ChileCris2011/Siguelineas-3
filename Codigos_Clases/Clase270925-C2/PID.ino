void PID(uint16_t position) {
  int error = position - 3500;
  integral += error;
  int derivative = error - lastError;
  int output = (Kp * error) + (Ki * integral) + (Kd * derivative);
  lastError = error;

  int vIzq = constrain(velocidadBaseIzq + output, 0, 255);
  int vDer = constrain(velocidadBaseDer - output, 0, 255);
  Motor(vIzq, vDer);
}

uint16_t posicion() {
  // Filtrado simple para el cálculo de posición
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] < 4000) sensorValues[i] = 0;
  }

  sumaPesada = 0;
  sumaTotal = 0;

  // Posición manual ponderada
  for (uint8_t i = 0; i < SensorCount; i++) {
    sumaPesada += (uint32_t)sensorValues[i] * (i * 1000);
    sumaTotal += sensorValues[i];
  }

  if (sumaTotal > 0) {
    return sumaPesada / sumaTotal;
  }

  return 0;
}

void gaps() {
  if (sumaTotal == 0) {
    // Avanza recto con velocidad base para no “morir” en líneas segmentadas
    Motor(velocidadBaseIzq - 10, velocidadBaseDer - 10);
    // Evita acumular integral/derivativos inútiles
    integral = 0;
    lastError = 0;
    // No corremos PID en esta iteración
    loop();
  }
}