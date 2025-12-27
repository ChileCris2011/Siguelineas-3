void inicializarGyro() {
  // Configuración MPU
  Wire.begin();
  delay(200);
  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU init error: ");
    Serial.println(status);
    while (true) {
      blink(1, 500, 1000);
    }
  }

  mpu.calcGyroOffsets();
}