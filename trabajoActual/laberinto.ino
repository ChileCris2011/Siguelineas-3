void laberinto() {
  delay(1000);
  while (true) {
    delay(1000);
    lab = true;
    Motor(50, 46);
    qtr.read(sensorValues);
    Serial.print("Distance in mm: ");
    distancia = lox.readRange();
    Serial.println(distancia);
    if (distancia < 160) {
      Motor(0, 0);
      lox.stopRangeContinuous();
      girarDerecha(87);
      delay(200);
      lox.startRangeContinuous();
      while (!lox.isRangeComplete());
      distancia = lox.readRange();
      if (distancia < 250) {
        Motor(0, 0);
        lox.stopRangeContinuous();
        girarIzquierda(175);
        delay(200);
        lox.startRangeContinuous();
        while (!lox.isRangeComplete())
          ;
        distancia = lox.readRange();
        if (distancia < 125) {
          Motor(0, 0);
          lox.stopRangeContinuous();
          girarIzquierda(87);
          delay(200);
          lox.startRangeContinuous();
          while (!lox.isRangeComplete())
            ;
          distancia = lox.readRange();
        }
      }
    }
    qtr.read(sensorValues);
    if ((sensorValues[2] >= umbral) || (sensorValues[5] >= umbral)) {
      Motor(0, 0);
      delay(500);
      break;
    }
  }
}
