#include "Pines.h"

const int freq = 5000;
const int resolution = 8;

void setup() {
  inicializarMotores();
  while (digitalRead(BOTON) == 0) {
  }
}
void loop() {
  Motor(100, 100);
}