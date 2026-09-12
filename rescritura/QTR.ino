void iniciarQTR(bool calibrar = false) {
  qtr.setTypeAnalog();
  qtr.setSensorPins(pinesQTR, sensorCount);
  qtr.setEmitterPin(27);

  if (calibrar) {
    Serial.println("Calibrando sensores...");
    for (uint16_t i = 0; i < 150; i++) qtr.calibrate();
    Serial.println("Listo QTR.");
  }
  delay(200);
}