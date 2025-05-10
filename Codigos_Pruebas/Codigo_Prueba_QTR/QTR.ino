void inicializarQTR() {  //inicializar QTR
  sigueLineas.setTypeAnalog();
  sigueLineas.setSensorPins((const uint8_t[]){
                              36, 39, 34, 35, 32, 33, 25, 26 },
                            SensorCount);
  sigueLineas.setEmitterPin(27);
}