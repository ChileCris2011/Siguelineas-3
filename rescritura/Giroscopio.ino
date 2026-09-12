void iniciarGiroscopio() {
  Wire.begin();
  delay(1000);
  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU init error: ");
    Serial.println(status);
    while (true) {
      blink(1000, 1000);
      blink(500, 500);    // Patron error giroscopio
      blink(1000, 1000);  // L - C - L - C
      blink(500, 2000);
    }
  }
  Serial.println("Calibrando giroscopio...");
  mpu.calcGyroOffsets();
  Serial.println("Listo MPU.");
}
