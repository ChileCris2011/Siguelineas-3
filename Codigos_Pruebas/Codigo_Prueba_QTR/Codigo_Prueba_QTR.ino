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
}
void loop() {
  sigueLineas.read(sensorValues);
  for (int i = 0; i < 8; i++){
    SerialBT.print(sensorValues[i]);
  }
}