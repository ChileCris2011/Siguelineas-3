void PID(uint16_t position) {
  float error = position - 3500;

  // Integra solo cuando esta cerca del centro para no acumular error al perder la linea.
  if (abs(error) < 300) {
    integral += error;
    integral = constrain(integral, -500, 500);
  }

  float derivative = error - lastError;

  float output = Kp * error + Ki * integral + Kd * derivative;

  lastError = error;

  int vIzq = constrain(velocidadBaseIzq - (int)output, 0, 255);
  int vDer = constrain(velocidadBaseDer + (int)output, 0, 255);

  Motor(vIzq, vDer);
}
