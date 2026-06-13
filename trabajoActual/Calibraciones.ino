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

  pinMode(XSHUT0, OUTPUT);
  pinMode(XSHUT1, OUTPUT);

  digitalWrite(XSHUT0, LOW);
  digitalWrite(XSHUT1, LOW);

  delay(100);

  digitalWrite(XSHUT0, HIGH);
  delay(10);

  // Configuración Laser 1
  if (!lox.begin(LOX0AD)) {
    Serial.println(F("No se encontró L0X 1!"));
    while (1) {
      digitalWrite(LED, HIGH);
      delay(1000);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(500);
    }
  }

  digitalWrite(XSHUT1, HIGH);
  delay(10);

  // Configuración Laser 2
  if (!lox1.begin(LOX1AD)) {
    Serial.println(F("No se encontró L0X 2!"));
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
    Serial.print("MPU init error: ");
    Serial.println(status);
    while (true) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(1000);
    }
  }
  Serial.println("Calibrando giroscopio...");
  mpu.calcGyroOffsets();
  Serial.println("Listo MPU.");
}
