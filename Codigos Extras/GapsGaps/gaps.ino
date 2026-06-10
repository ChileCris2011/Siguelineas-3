void gaps() {
  digitalWrite(LED, HIGH);

  while (true) {
    Motor(50, 50);
    delay(200);

    Motor(0, 0);
    delay(100);

    if (girarDerecha(60, true) == 1) {
      break;
    }

    if (girarIzquierda(120, true) == 1) {
      break;
    }

    if (girarDerecha(60, true) == 1) {
      break;
    }
  }

  Motor(50, 50);
  delay(200);

  digitalWrite(LED, LOW);
}