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

const int freq = 5000;
const int resolution = 8;

const int umbral = 3333;

int intersecciones = 0;


void setup() {
  Serial.begin(9600);
  inicializarQTR();
  inicializarMotores();
  SerialBT.begin("3 Absoluto");
  while (digitalRead(BOTON) == 0) {
  }
}
void loop() {
  sigueLineas.read(sensorValues);
  /*
    for (int i = 0; i < 8; i++) {
    SerialBT.print(sensorValues[i]);
    SerialBT.print('\t');
    }
    SerialBT.print(intersecciones);
    SerialBT.print('\n');
  */
  if (sensorValues[0] > umbral && sensorValues[7] > umbral) {  // N, x, x, x, x, x, x, N
    intersecciones++;
    SerialBT.println(intersecciones);
    Motor(75, 75);
    delay(333);
  } else if (sensorValues[0] > umbral && sensorValues[1] > umbral && sensorValues[6] < umbral && sensorValues[7] < umbral) {  // N, N, x, x, x, x, B, B
    IZQ90();
  } else if (sensorValues[0] < umbral && sensorValues[1] < umbral && sensorValues[6] > umbral && sensorValues[6] > umbral) {  // B, B, x, x, x, x, N, N
    DER90();
  } else {
    seguidorDeLineas();
  }
}
/*
* Para añadir un cuadrado con indicador:
*    indicador(x);
* Siendo x la cantidad de intersecciones anteriores + 1
*/