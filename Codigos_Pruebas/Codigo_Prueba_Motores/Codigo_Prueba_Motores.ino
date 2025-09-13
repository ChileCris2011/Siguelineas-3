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
  delay(750);
  Motor(-100, -100);
  delay(750);
  Motor(-100, 100);
  delay(750);
  Motor(100, -100);
  delay(750);
  Motor(0, 0);
  while (digitalRead(BOTON) == 0) {
  }
}
