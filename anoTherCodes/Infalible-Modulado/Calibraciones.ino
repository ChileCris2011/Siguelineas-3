void calibracionSensores(){
   // Configuración QTR
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){ 36, 39, 34, 35, 32, 33, 25, 26 }, SensorCount);
  qtr.setEmitterPin(27);

  // Calibración QTR
  Serial.println("Esperando botón para calibrar QTR...");
  while (digitalRead(BOTON) == LOW) delay(10);
  Serial.println("Calibrando sensores...");
  for (uint16_t i = 0; i < 150; i++) qtr.calibrate();
  Serial.println("Listo QTR.");
}
void calibracionGiroscopio(){
   // Configuración MPU
  Wire.begin();
  delay(200);
  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU init error: ");
    Serial.println(status);
    while (true) delay(1000);
  }
  Serial.println("Calibrando giroscopio...");
  mpu.calcGyroOffsets();
  Serial.println("Listo MPU.");
}