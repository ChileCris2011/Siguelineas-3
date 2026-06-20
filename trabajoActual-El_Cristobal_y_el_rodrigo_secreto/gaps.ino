int giroconveniente = 0;
int counter = 0;

void gaps() {
  digitalWrite(LED, HIGH);

  while (true) {
    Motor(50, 50);
    delay(144);

    Motor(0, 0);
    delay(100);

    if (counter < 2) {
      if (giroconveniente >= 0) {
        if (girarDerecha(60, true) == 1) {
          giroconveniente = 1;
          break;
        }

        if (girarIzquierda(60, true) == 1) {
          break;
        }

        if (girarIzquierda(60, true) == 1) {
          giroconveniente = -1;
          break;
        }

        if (girarDerecha(60, true) == 1) {
          break;
        }
      } else if (giroconveniente == -1) {
        if (girarIzquierda(60, true) == 1) {
          giroconveniente = -1;
          break;
        }

        if (girarDerecha(60, true) == 1) {
          break;
        }

        if (girarDerecha(60, true) == 1) {
          giroconveniente = 1;
          break;
        }

        if (girarIzquierda(60, true) == 1) {
          break;
        }
      }
    } else {
      if (giroconveniente >= 0) {
        if (girarDerecha(145, true) == 1) {
          giroconveniente = 1;
          break;
        }

        if (girarIzquierda(145, true) == 1) {
          break;
        }

        if (girarIzquierda(145, true) == 1) {
          giroconveniente = -1;
          break;
        }

        if (girarDerecha(145, true) == 1) {
          break;
        }
      } else if (giroconveniente == -1) {
        if (girarIzquierda(145, true) == 1) {
          giroconveniente = -1;
          break;
        }

        if (girarDerecha(145, true) == 1) {
          break;
        }

        if (girarDerecha(145, true) == 1) {
          giroconveniente = 1;
          break;
        }

        if (girarIzquierda(145, true) == 1) {
          break;
        }
      }
      counter = 0;
    }
    counter++;
  }

  Motor(50, 50);
  delay(200);

  counter = 0;

  digitalWrite(LED, LOW);
}