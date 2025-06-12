void fMarcas(int der) {
  for (int i = 0; i < lIntersecciones; i++) {
    if (contador == intersecciones[i]) {
      pass();
      return;
    }
  }

  if (!inCuadrado) {
    switch (contador) {
      case primeraMarca:
        primerGiro = der;
        pass();
        break;
      case segundaMarca:
        segundoGiro = der;
        pass();
        break;
      case primerCuadrado:
        if (primerGiro) {
          giroDer();
        } else {
          giroIzq();
        }
        inCuadrado = true;
        break;
      case segundoCuadrado:
        if (segundoGiro) {
          giroDer();
        } else {
          giroIzq();
        }
        inCuadrado = true;
        break;
      case hFinal:
        Motor(0, 0);
        while (true) {
          delay(200);
          digitalWrite(LED, HIGH);
          delay(200);
          digitalWrite(LED, LOW);
        }
      default:
        girar(der);
    }
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    contador += 1;
  } else {
    if ((der == 0 && (primerGiro == 0 || segundoGiro == 0)) || (der == 1 && (primerGiro == 1 || segundoGiro == 1))) {
      inCuadrado = false;
    }
  }
}