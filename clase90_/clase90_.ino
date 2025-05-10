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

const int umbral = 3000;

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
  if (sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] < umbral && sensorValues[4] < umbral && sensorValues[5] < umbral && sensorValues[6] < umbral) {
    Motor(65, 65);
  } else if (sensorValues[1] > umbral && sensorValues[2] > umbral && sensorValues[3] > umbral && sensorValues[4] > umbral && sensorValues[5] > umbral && sensorValues[6] > umbral) {
    intersecciones++;
    Motor(75, 75);
    delay(333);
  }  else if (sensorValues[0] > umbral && sensorValues[1] > umbral && sensorValues[2] > umbral && sensorValues[3]  > umbral && sensorValues[4] > umbral && sensorValues[5] < umbral && sensorValues[6] < umbral && sensorValues[7] < umbral) {
    Motor(0, 0);
    delay(1000);
    Motor(-50,150);
    delay(1000);
    SerialBT.print("izf");
    //izF
  }
  else if (sensorValues[0] < umbral && sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] > umbral && sensorValues[4] > umbral && sensorValues[5] > umbral && sensorValues[6] > umbral && sensorValues[7] > umbral) {
    Motor(0, 0);
    delay(1000);
    Motor(150, -50);
    delay(1000);
        SerialBT.print("der");
    //deF
  }
  
}
/*
  if (sensorValues[0] > umbral) {
    Motor(10, -100);
  } else if (sensorValues[7] > umbral) {
    Motor(-100, 10);
  }

    Motor(50, 50);
    delay(666);
    Motor(-60, 60);
    delay(1333);
    Motor(50, 50);
    delay(100);

    Motor(50, 50);
    delay(666);
    Motor(60, -60);
    delay(1333);
    Motor(50, 50);
    delay(100);
*/
