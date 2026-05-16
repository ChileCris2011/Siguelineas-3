void inicializarSensores(){
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){ 36, 39, 34, 35, 32, 33, 25, 26 }, SensorCount);
  qtr.setEmitterPin(27);
}

void calibrarSensores(){  
  while (digitalRead(BOTON) == 0)
    ;
  for (int i = 0; i < 200; i++) {
    qtr.calibrate();
    digitalWrite(LED, HIGH);
    delay(10);
    digitalWrite(LED, LOW);
    delay(10);
  }
  while (digitalRead(BOTON) == 0)
    ;
}
