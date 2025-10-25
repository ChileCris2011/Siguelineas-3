const int distDetec = 100;

int dishtancia = 0;

void labirint() {
  bool line = false;
  delay(1000);
  digitalWrite(LED, HIGH);
  while (true) {
    while (true) {
      dishtancia = 0;
      if (lox.isRangeComplete()) {
        dishtancia = lox.readRange();
      }
      SerialBT.println(dishtancia);
      Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
      qtr.read(sensorValues);
      if (sensorValues[0] > 3800) {
        line = true;
        while (sensorValues[7] < 3800) {
          qtr.read(sensorValues);
          Motor(0, 50);
        }
        Motor(0, 0);
        break;
      } else if (sensorValues[7] > 3800) {
        line = true;
        while (sensorValues[0] < 3800) {
          qtr.read(sensorValues);
          Motor(50, 0);
        }
        Motor(0, 0);
        break;
      }
      if (dishtancia < distDetec && dishtancia > distDetec - 25) {
        break;
      }
    }
    if (line) {
      while (sensorValues[0] > 3800 && sensorValues[7] > 3800) {
        qtr.read(sensorValues);
        Motor(velocidadBaseIzq - (velocidadBaseIzq - 10), velocidadBaseDer - (velocidadBaseDer - 10));
      }
      blockLabirint = true;
      break;
    }

    SerialBT.print("Labirint | Giro 90, ");
    SerialBT.println(dishtancia);

    girarIzquierda(85);

    Motor(0, 0);
    delay(1000);

    dishtancia = 0;
    if (lox.isRangeComplete()) {
      dishtancia = lox.readRange();
    }
    if (dishtancia < distDetec && dishtancia > distDetec - 25) {
      girarDerecha(175);
      SerialBT.print("Labirint | Giro 180, ");
      SerialBT.println(dishtancia);
    }
  }
}