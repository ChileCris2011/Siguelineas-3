void parpadeo(int rep, int del) {
  for (int i = 0, i < rep; i++) {
    digitalWrite(LED, HIGH);
    delay(del);
    digitalWrite(LED, LOW);
    delay(del);
  }
}

void esperarBoton() {
  while (digitalRead(BOTON) == LOW) delay(10);
}