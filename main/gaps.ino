// Recuerda hacia que lado encontro la linea la ultima vez:
//  1 derecha, -1 izquierda, 0 sin preferencia.
int direccionPreferidaGap = 0;
int intentosBusquedaGap = 0;

void gaps() {
  digitalWrite(LED, HIGH);

  while (true) {
    Motor(50, 50);
    delay(144);

    Motor(0, 0);
    delay(100);

    // Primero prueba barridos cortos. Si no aparece la linea, abre mas el angulo.
    if (intentosBusquedaGap < 2) {
      if (direccionPreferidaGap >= 0) {
        if (girarDerecha(60, true) == 1) {
          direccionPreferidaGap = 1;
          break;
        }

        if (girarIzquierda(60, true) == 1) {
          break;
        }

        if (girarIzquierda(60, true) == 1) {
          direccionPreferidaGap = -1;
          break;
        }

        if (girarDerecha(60, true) == 1) {
          break;
        }
      } else if (direccionPreferidaGap == -1) {
        if (girarIzquierda(60, true) == 1) {
          direccionPreferidaGap = -1;
          break;
        }

        if (girarDerecha(60, true) == 1) {
          break;
        }

        if (girarDerecha(60, true) == 1) {
          direccionPreferidaGap = 1;
          break;
        }

        if (girarIzquierda(60, true) == 1) {
          break;
        }
      }
      intentosBusquedaGap++;
    } else {
      if (direccionPreferidaGap >= 0) {
        if (girarDerecha(145, true) == 1) {
          direccionPreferidaGap = 1;
          break;
        }

        if (girarIzquierda(145, true) == 1) {
          break;
        }

        if (girarIzquierda(145, true) == 1) {
          direccionPreferidaGap = -1;
          break;
        }

        if (girarDerecha(145, true) == 1) {
          break;
        }
      } else if (direccionPreferidaGap == -1) {
        if (girarIzquierda(145, true) == 1) {
          direccionPreferidaGap = -1;
          break;
        }

        if (girarDerecha(145, true) == 1) {
          break;
        }

        if (girarDerecha(145, true) == 1) {
          direccionPreferidaGap = 1;
          break;
        }

        if (girarIzquierda(145, true) == 1) {
          break;
        }
      }
      intentosBusquedaGap = 0;
    }
  }

  Motor(50, 50);
  delay(200);

  Motor(0, 0);
  delay(500);

  intentosBusquedaGap = 0;

  digitalWrite(LED, LOW);
  delay(200);
}
