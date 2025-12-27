void inicializarQTR(bool calibrar = false;) {
  qtr.setTypeAnalog();                 // QTR en modo análogo (lecturas/pins analogicos)
  qtr.setSensorPins((const uint8_t[]){ // Configurar pines QTR
                                       36, 39, 34, 35, 32, 33, 25, 26 },
                    SensorCount);
  qtr.setEmitterPin(27);        // Emitter pin
  if (calibrar) calibrarQTR();  // Calibrar
}

void calibrarQTR(uint16_t t = 150) {
  for (uint16_t i = 0; i < t; i++) qtr.calibrate();
}