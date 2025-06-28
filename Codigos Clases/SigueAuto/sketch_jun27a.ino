#include "pines.h"

#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <QTRSensors.h>


const int frecuencia = 5000;
const int resolution = 8;
const int umbral = 3500;

int primerCuadrado = -1;
int segundoCuadrado = -1;

QTRSensors qtr;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
int ultimaPosicion = 0;

void setup() {
  Serial.begin(115200);

  inicializarMotores();

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){ 36, 39, 34, 35, 32, 33, 25, 26 }, SensorCount);
  qtr.setEmitterPin(27);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  if (!lox.begin()) {
    Serial.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1) {
      digitalWrite(LED, HIGH);
      delay(1000);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(1000);
    }
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

  Serial.begin(115200);


  while (digitalRead(BOTON) == 0) {
  }
  delay(500);
}

void loop() {

  ///////////////////////////////// Obstáculo /////////////////////////////////

  VL53L0X_RangingMeasurementData_t measure;  //Declara la variable para almacenar los datos de la medición

  //Realiza la medición de distancia y almacena los resultados en la variable 'measure'
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {
    if (measure.RangeMilliMeter < 100) {
      Motor(-50, -50);
      delay(500);
      Motor(0, 0);
      delay(200);
      Motor(80, -80);
      delay(700);
      Motor(50, 50);
      delay(1500);
      Motor(0, 0);
      delay(200);
      Motor(-80, 80);
      delay(700);
      Motor(50, 50);
      delay(3250);
      Motor(0, 0);
      delay(200);
      Motor(-80, 80);
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
        Motor(80, -80);
      }
      Motor(50, 50);
      delay(200);
      Motor(0, 0);
    }
  }

  ///////////////////////////////// Giros 90 | Intersecciones /////////////////////////////////

  qtr.read(sensorValues);
  int dreccion = 0;
  if (sensorValues[0] < umbral && sensorValues[3] > umbral && sensorValues[4] > umbral && sensorValues[7] > umbral) {
    interseccion();
  } else if (sensorValues[0] > umbral && sensorValues[1] > umbral && sensorValues[2] > umbral && sensorValues[3] > umbral && sensorValues[4] > umbral) {  //IZQUIERDA
    giros(Izquierda);
  } else if (sensorValues[7] > umbral && sensorValues[6] > umbral && sensorValues[5] > umbral && sensorValues[4] > umbral && sensorValues[3] > umbral) {  //DERECHA
    giros(Derecha);
  }

  ///////////////////////////////// Corrección /////////////////////////////////

  // Verificar si todos los sensores están "apagados" (sin línea)
  bool sinLinea = true;
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] > umbral) {
      sinLinea = false;
      break;
    }
  }

  int posicion;
  if (sinLinea) {
    if (ultimaPosicion > 240) {
      posicion = 255;
    } else if (ultimaPosicion < -240) {
      posicion = -255;
    } else {
      posicion = 0;  // línea perdida sin dirección clara
    }
  }

  ///////////////////////////////// Sigue Líneas /////////////////////////////////

  else {
    posicion = qtr.readLineBlack(sensorValues);
    posicion = map(posicion, 0, 7000, -1000, 1000);
    posicion = constrain(posicion, -255, 255);
    ultimaPosicion = posicion;
  }

  if (posicion >= -50 && posicion <= 50) {
    Motor(50, 50);
  } else if (posicion < -50 && posicion > -150) {
    Motor(0, 80);
  } else if (posicion > 50 && posicion < 150) {
    Motor(80, 0);
  } else if (posicion > 150) {  //seguidor
    Motor(120, 0);
  } else if (posicion < -150) {
    Motor(0, 120);
  }
}