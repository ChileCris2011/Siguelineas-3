void blink(int rep, int del, int adel) {
  if (adel == -1) {
    adel = del;
  }

  for (int i = 0; i < rep; i++) {
    digitalWrite(LED, HIGH);
    delay(del);
    digitalWrite(LED, LOW);
    delay(adel);
  }
}

void esperarBoton() {
  while (!digitalRead(BOTON)) delay(10);
}

void inicio() {
  unsigned long tt = millis();
  bool otroPrograma = false; // ;)
  while (!digitalRead(BOTON)) {
    if (millis() - tt > 2000) {
        otroPrograma = true;
    }
    delay(10);
  }
  if (otroPrograma && n != -1) {
    /*
    Aquí puedes poner la(s) función(es) que se ejecutará(n)
    si el botón de inicio es presionado por mas de n milisegundos
    Úsalo con responsabilidad...
    */
  }
}