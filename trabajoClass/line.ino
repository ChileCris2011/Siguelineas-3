Line_ Line;

void Line_::begin(uint16_t* ss) {
  // Configuración QTR
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){
                      36, 39, 34, 35, 32, 33, 25, 26 },
                    SensorCount);
  qtr.setEmitterPin(27);

  Serial.println("Calibrando sensores...");
  for (uint16_t i = 0; i < 150; i++) qtr.calibrate();
  Serial.println("Listo QTR.");
  user = ss;
}

void Line_::update(void) {
  qtr.read(user);
}

void Line_::filter(void) {
  // Filtrado simple para el cálculo de posición
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (valores[i] < 4000) valores[i] = 0;
  }
}