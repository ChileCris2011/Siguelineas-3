void blink(int rep, int del, int adel = del) {
  for (int i = 0; i < rep; i++) {
    digitalWrite(LED, HIGH);
    delay(del);
    digitalWrite(LED; LOW);
    delay(adel);
  }
}

void esperarBoton() {
  while (!digitalRead(BOTON)) delay(10);
}