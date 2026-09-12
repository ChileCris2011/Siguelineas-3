#include "header.h"

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(BOTON, INPUT_PULLDOWN);

  iniciarMotores();

  esperarBoton();
  iniciarQTR();
  iniciarLaser();

  blink(500);

  esperarBoton();
  iniciarGiroscopio();
}

void loop() {
  detectarObstaculo();
}
