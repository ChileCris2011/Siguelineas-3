#include "Pines.h"
#include <QTRSensors.h>
#include <BluetoothSerial.h>

QTRSensors sigueLineas;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Fallo de bluetooth
#endif

BluetoothSerial SerialBT;

const uint8_t SensorCount = 8;

uint16_t sensorValues[SensorCount];

void setup() {
  inicializarQTR();
  SerialBT.begin("|3|");
  while (digitalRead(BOTON) == 0) {
  }
  for (int i = 0; i < 100; i++) {
    sigueLineas.calibrate();
    digitalWrite(LED, HIGH);
    delay(10);
    digitalWrite(LED, LOW);
    delay(10);
  }
  while (digitalRead(BOTON) == 0) {
  }
}
void loop() {
  sigueLineas.read(sensorValues);
  for (int i = 0; i < 8; i++) {
    SerialBT.print(sensorValues[i]);
    SerialBT.print("  ");
  }
  SerialBT.print(" | ");

  int posicion = sigueLineas.readLineBlack(sensorValues);
  posicion = map(posicion, 0, 7000, -1000, 1000);  // Se mapea a un valor entre -255 y 255 (No se que pasa aqui)
  posicion = constrain(posicion, -255, 255);
  SerialBT.println(posicion);
}