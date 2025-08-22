void forzarNextSemi(bool vioIzq, bool vioDer) {
  if (vioIzq)  {
    girarIzquierda(80);
  }
  else         {
    girarDerecha(80);
  }
  forzarProximaSemi = false;  // consumir la orden
  puedeLaser = true;
}

void guardarMarca(bool vioIzq, bool vioDer) {
  if (totalMarcasGuardadas < 2) {
    if (vioIzq)  {
      marcaCuadradoDir[totalMarcasGuardadas] = -1;
    }
    if (vioDer)  {
      marcaCuadradoDir[totalMarcasGuardadas] = +1;
    }
    totalMarcasGuardadas++;
    tieneMarcaCuadrado = true;
  }
}

void girosNoventa(bool vioIzq, bool vioDer) {
  if (vioIzq)  {
    girarIzquierda(80);
    return;
  }
  if (vioDer)  {
    girarDerecha(80);
    return;
  }
}

void entrarCuadrado() {
  int dir = marcaCuadradoDir[0];

  // Desplazar las marcas para que la segunda pase a ser primera
  for (int i = 0; i < totalMarcasGuardadas - 1; i++) {
    marcaCuadradoDir[i] = marcaCuadradoDir[i + 1];
  }
  totalMarcasGuardadas--;

  // Ejecutar el giro
  Motor(40, 40);
  delay(666);
  if (dir < 0) girarIzquierda(80);
  else          girarDerecha(80);

  // Preparar forzado en la próxima semi si aún queda otra marca
  forzarProximaSemi = true;
  puedeLaser = false;
}
void finalizar() {
  Motor(0, 0);
  delay(1000);
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    delay(1000);
  }
  while (true) {} // fin
}
