void esperarBoton() {
  while (!digitalRead(BOTON)) delay(10);
}

void blink(unsigned long tiempoEncendido, unsigned long tiempoApagado = 0) {
  digitalWrite(LED, HIGH);
  delay(tiempoEncendido);
  digitalWrite(LED, LOW);
  delay(tiempoApagado);
}