#include "Pines.h"
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

//#include <BluetoothSerial.h>

// Crea una instancia del sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//BluetoothSerial SerialBT;

const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  //SerialBT.begin("Valor Absoluto de 3");

  Serial.println("Iniciando...");

  inicializarMotores();

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
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;  //Declara la variable para almacenar los datos de la medición

  //Realiza la medición de distancia y almacena los resultados en la variable 'measure'
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {
    if (measure.RangeMilliMeter < 100) {
      Motor(50, -50);
      delay(1000);
      Motor(-50, 50);
      delay(1000);
      Motor(-50, -50);
      delay(1000);
      Motor(50, 50);
      delay(1000);
      Motor(0, 0);
      delay(1000);
    }
  }
}