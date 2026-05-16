const int distDetec = 166;

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
      Motor(velocidadBaseIzq, velocidadBaseDer);
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
      SerialBT.println("¡Final!");
      Motor(0, 0);
      delay(500);  // Se detiene
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED, HIGH);  // Juego de luces (3)
        delay(500);
        digitalWrite(LED, LOW);
        delay(500);
      }
      while (true) {}  // fin
    }

    SerialBT.print("Labirint | Giro 90, ");
    SerialBT.println(dishtancia);

    girarIzquierda(90);

    Motor(0, 0);
    delay(1000);
  }
}