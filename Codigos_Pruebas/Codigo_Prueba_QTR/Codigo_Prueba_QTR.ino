#include "Pines.h"
#include <QTRSensors.h>
#include <BluetoothSerial.h>

QTRSensors sigueLineas;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth no esta activado o la placa no lo contiene
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

  // Filtrado simple para el cálculo de posición
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] < 3500) sensorValues[i] = 0;
    SerialBT.print(sensorValues[i]);
    SerialBT.print(" ");
  }

  SerialBT.print("| ");

  // Posición manual ponderada
  uint32_t sumaPesada = 0;
  uint32_t sumaTotal  = 0;
  for (uint8_t i = 0; i < SensorCount; i++) {
    sumaPesada += (uint32_t)sensorValues[i] * (i * 1000);
    sumaTotal  += sensorValues[i];
  }
  uint16_t position = (sumaTotal > 0) ? (sumaPesada / sumaTotal) : 0;

  SerialBT.println(position);
  delay(100);

}