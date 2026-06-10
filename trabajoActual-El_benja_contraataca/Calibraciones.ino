void calibracionSensores() {
  // Configuración QTR
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){
                      36, 39, 34, 35, 32, 33, 25, 26 },
                    SensorCount);
  qtr.setEmitterPin(27);

  // Calibración QTR
  elog("Esperando botón para calibrar QTR...");
  while (digitalRead(BOTON) == LOW) delay(10);
  elog("Calibrando sensores...");
  for (uint16_t i = 0; i < 150; i++) qtr.calibrate();
  elog("Listo QTR.");

  // Configuración Laser
  if (!lox.begin()) {
    errorLog("¡Error de inicio de V53L0X! Verifica las conexiones.");
    while (1) {
      digitalWrite(LED, HIGH);
      delay(1000);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(500);
    }
  }
  lox.startRangeContinuous();  // Empieza la lectura contínua del láser
}
void calibracionGiroscopio() {
  // Configuración MPU
  Wire.begin();
  delay(200);
  byte status = mpu.begin();
  if (status != 0) {
    errorLog("MPU init error: ", true, false);
    errorLog(status, false)
    while (true) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(1000);
    }
  }
  elog("Calibrando giroscopio...");
  mpu.calcGyroOffsets();
  elog("Listo MPU.");
}
