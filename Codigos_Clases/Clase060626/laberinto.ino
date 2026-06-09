const int distDetec = 166;

int dishtancia = 0;

void labirint() {
  bool line = false;
  delay(1000);
  digitalWrite(LED, HIGH);
  while (true) {
    qtr.read(sensorValues);
    Motor(-20, -20);
    if (sensorValues[0] > 4000) {
      while (true) {
        qtr.read(sensorValues);
        Motor(0, -10);
        if (sensorValues[7] > 4000) {
          Motor(0, 0);
          break;
        }
      }
      break;
    }
    if (sensorValues[7] > 4000) {
      while (true) {
        qtr.read(sensorValues);
        Motor(-10, 0);
        if (sensorValues[0] > 4000) {
          Motor(0, 0);
          break;
        }
      }
      break;
    }
  }
  while (sensorValues[0] > 4000 && sensorValues[7] > 4000) {
    qtr.read(sensorValues);
    Motor(20, 20);
  }
  digitalWrite(LED, LOW);
  delay(200);
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
      delay(25);
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

    girarIzquierda(84);

    Motor(0, 0);
    delay(1000);

    dishtancia = 0;
    if (lox.isRangeComplete()) {
      dishtancia = lox.readRange();
    }
    if (dishtancia < distDetec && dishtancia > distDetec - 25) {
      SerialBT.print("Labirint | Giro 180, ");
      SerialBT.println(dishtancia);
      
      girarDerecha(175);
    }
  }
}