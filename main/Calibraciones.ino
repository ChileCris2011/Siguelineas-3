void calibracionSensores() {
  // Configuración QTR
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){
                      36, 39, 34, 35, 32, 33, 25, 26 },
                    SensorCount);
  qtr.setEmitterPin(27);

  // Calibración QTR
  Serial.println("Esperando botón para calibrar QTR...");
  while (digitalRead(BOTON) == LOW) delay(10);
  Serial.println("Calibrando sensores...");
  for (uint16_t i = 0; i < 150; i++) qtr.calibrate();
  Serial.println("Listo QTR.");

  delay(1000);

  // Configuración Laser
  if (!lox.begin()) {
    Serial.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1) {
      digitalWrite(LED, HIGH);
      delay(2000);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(500);
    }
  }
  lox.startRangeContinuous();  // Empieza la lectura contínua del láser

  Wire.begin();

  // --- Inicializar sensor 1 ---
  muxSelect(CANAL_SENSOR_1);
  if (!rgb1.begin()) {
    while (true) {
      digitalWrite(LED, HIGH);
      delay(5000);
      digitalWrite(LED, LOW);
      delay(500);
    }
  }

  // --- Inicializar sensor 2 ---
  muxSelect(CANAL_SENSOR_2);
  if (!rgb2.begin()) {
    while (true) {
      digitalWrite(LED, HIGH);
      delay(1000);
    }
  }
}

void calibracionGiroscopio() {
  // Configuración MPU
  Wire.begin();
  delay(200);
  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU init error: ");
    Serial.println(status);
    while (true) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(2000);
    }
  }
  Serial.println("Calibrando giroscopio...");
  mpu.calcGyroOffsets();
  Serial.println("Listo MPU.");
}
