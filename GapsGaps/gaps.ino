void gaps() {
  delay(500);
  while (true) {
    Motor(50, 50);
    delay(200);

    Motor(0, 0);
    delay(200);

    if (girarIzquierda(30, true) == 1) {
      return;
    }

    if (girarDerecha(60, true) == 1) {
      return;
    }

    if (girarIzquierda(30, true) == 1) {
      return;
    }
  }
}