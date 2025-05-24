void inicializarQTR() {  //inicializar QTR
  sigueLineas.setTypeAnalog();
  sigueLineas.setSensorPins((const uint8_t[]){
                              36, 39, 34, 35, 32, 33, 25, 26 },
                            SensorCount);
  sigueLineas.setEmitterPin(27);
}

void seguidorDeLineas() {
  if (sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] < umbral && sensorValues[4] < umbral && sensorValues[5] < umbral && sensorValues[6] < umbral) {  // -----> x, B, B, B, B, B, B, x
    Motor(65, 65);
  } else if (sensorValues[1] > umbral && sensorValues[2] > umbral && sensorValues[3] < umbral && sensorValues[4] < umbral && sensorValues[5] < umbral && sensorValues[6] < umbral) {  // x, N, N, B, B, B, B, x
    Motor(0, 80);
  } else if (sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] < umbral && sensorValues[4] < umbral && sensorValues[5] > umbral && sensorValues[6] > umbral) {  // x, B, B, B, B, N, N, x
    Motor(80, 0);
  } else if (sensorValues[1] < umbral && sensorValues[2] > umbral && sensorValues[3] > umbral && sensorValues[4] < umbral && sensorValues[5] < umbral && sensorValues[6] < umbral) {  // x, B, N, N, B, B, B, x
    Motor(0, 60);
  } else if (sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] < umbral && sensorValues[4] > umbral && sensorValues[5] > umbral && sensorValues[6] < umbral) {  // x, B, B, B, N, N, B, x
    Motor(60, 0);
  } else if (sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] > umbral && sensorValues[4] > umbral && sensorValues[5] < umbral && sensorValues[6] < umbral) {  // x, B, B, N, N, B, B, x
    Motor(80, 80);
  }
}
void IZQ90() {
  Motor(90, 90);
  delay(333);
  SerialBT.println("Giro de 90° Izquierda");
  while (!(sensorValues[2] > umbral && sensorValues[3] > umbral && sensorValues[4] < umbral && sensorValues[5] < umbral && sensorValues[6] < umbral)) {  // x, x, N, N, B, B, B, x)
    sigueLineas.read(sensorValues);
    Motor(160, -80);
  }
}


void DER90() {
  Motor(90, 90);
  delay(333);
  SerialBT.println("Giro de 90° Derecha");
  while (!(sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] < umbral && sensorValues[4] > umbral && sensorValues[5] > umbral)) {  // x, B, B, B, N, N, x, x)
    sigueLineas.read(sensorValues);
    Motor(-80, 160);
  }
}

//void indicador(int inter) {


/*
 if (sensorValues[1] > umbral && sensorValues[2] > umbral && sensorValues[3] < umbral && sensorValues[4] < umbral && sensorValues[5] < umbral && sensorValues[6] < umbral) {        // x, N, N, B, B, B, B, x
    Motor(0, 80);
  } else if (sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] < umbral && sensorValues[4] < umbral && sensorValues[5] > umbral && sensorValues[6] > umbral) { // x, B, B, B, B, N, N, x
    Motor(80, 0);
  } else if (sensorValues[1] < umbral && sensorValues[2] > umbral && sensorValues[3] > umbral && sensorValues[4] < umbral && sensorValues[5] < umbral && sensorValues[6] < umbral) { // x, B, N, N, B, B, B, x
    Motor(0, 65);
  } else if (sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] < umbral && sensorValues[4] > umbral && sensorValues[5] > umbral && sensorValues[6] < umbral) { // x, B, B, B, N, N, B, x
    Motor(65, 0);
  } else if (sensorValues[1] < umbral && sensorValues[2] > umbral && sensorValues[3] > umbral && sensorValues[4] < umbral && sensorValues[5] < umbral && sensorValues[6] < umbral) { // x, B, N, N, B, B, B, x
    Motor(0, 70);
  } else if (sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] > umbral && sensorValues[4] > umbral && sensorValues[5] > umbral && sensorValues[6] < umbral) { // x, B, B, B, N, N, B, x
    Motor(70, 0);
  }
  */
