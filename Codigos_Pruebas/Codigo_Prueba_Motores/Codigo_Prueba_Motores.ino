#include "Pines.h"

void setup() {
  inicializarMotores
  while (digitalRead(BOTON) == 0) {
  }
}
void loop() {
  Motor(100, 100)
}