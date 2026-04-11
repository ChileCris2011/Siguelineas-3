#include "definiciones.h"
void setupSensores() {
  //Aqui va la configuracion de los sensores
  //siguelineas
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){ 26,25, 33, 32, 35, 34, 39,36 }, SensorCount);
  qtr.setEmitterPin(27);  
}

void calibrar(){  
  while (digitalRead(BOTON) == 0)
    ;
  for (int i = 0; i < 200; i++) {
    qtr.calibrate();
    digitalWrite(LED, HIGH);
    delay(10);
    digitalWrite(LED, LOW);
    delay(10);
  }
  while (digitalRead(BOTON) == 0)
    ;
}
