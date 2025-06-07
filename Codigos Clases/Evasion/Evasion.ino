#include "Pines.h"
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <QTRSensors.h>

//#include <BluetoothSerial.h>

QTRSensors qtr;

// Crea una instancia del sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//BluetoothSerial SerialBT;

const int freq = 5000;
const int resolution = 8;
int umbral = 3500;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
int ultimaPosicion = 0;

void setup() {
  Serial.begin(115200);
  //SerialBT.begin("Valor Absoluto de 3");

  Serial.println("Iniciando...");

  inicializarMotores();

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){
                      36, 39, 34, 35, 32, 33, 25, 26 },
                    SensorCount);
  qtr.setEmitterPin(27);
  pinMode(LED, OUTPUT);

  // Inicia la comunicacion I2C con el sensor
  if (!lox.begin()) {
    Serial.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1)
      ;  // Detiene el programa si no se puede inicializar el sensor
  }

  Serial.println(F("VL53L0X iniciado correctamente"));
  while (digitalRead(BOTON) == 0) {
  }
  delay(500);

  for (int i = 0; i < 100; i++) {
    qtr.calibrate();
    digitalWrite(LED, HIGH);
    delay(10);
    digitalWrite(LED, LOW);
    delay(10);
  }

  while (digitalRead(BOTON) == 0) {
  }
  delay(500);
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;  //Declara la variable para almacenar los datos de la medición

  //Realiza la medición de distancia y almacena los resultados en la variable 'measure'
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {
    if (measure.RangeMilliMeter < 100) {
      Motor(-50, -50);
      delay(500);
      Motor(0, 0);
      delay(200);
      Motor(-80, 80);
      delay(700);
      Motor(50, 50);
      delay(1500);
      Motor(0, 0);
      delay(200);
      Motor(80, -80);
      delay(700);
      Motor(50, 50);
      delay(3250);
      Motor(0, 0);
      delay(200);
      Motor(80, -80);
      delay(700);
      while (sensorValues[3] < umbral || sensorValues[4] < umbral) {
        qtr.read(sensorValues);
        Motor(50, 50);
      }
      Motor(0, 0);
      delay(200);
      Motor(50, 50);
      delay(250);
      qtr.read(sensorValues);
      while (sensorValues[4] < umbral) {
        qtr.read(sensorValues);
        Motor(-80, 80);
      }
      Motor(50, 50);
      delay(200);
      Motor(0, 0);
    }
  }
}