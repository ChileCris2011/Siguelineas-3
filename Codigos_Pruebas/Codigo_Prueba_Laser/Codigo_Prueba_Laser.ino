#include <Wire.h>
#include <Adafruit_VL53L0X.h>

#include <BluetoothSerial.h>

// Crea una instancia del sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

BluetoothSerial SerialBT;

void setup() {
  SerialBT.println("Iniciando...");

  SerialBT.begin("|3|");

  // Inicia la comunicacion I2C con el sensor
  if (!lox.begin()) {
    SerialBT.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1);  // Detiene el programa si no se puede inicializar el sensor
  }

  SerialBT.println(F("VL53L0X iniciado correctamente"));
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;  //Declara la variable para almacenar los datos de la medición

  //Realiza la medición de distancia y almacena los resultados en la variable 'measure'
  lox.rangingTest(&measure, false);

  //Verifica si la medición es válida
  if (measure.RangeStatus != 4) {  // Si el estado es diferente a 4, la medición es válida
    SerialBT.print("Distancia: ");
    SerialBT.print(measure.RangeMilliMeter)  // Imprime la distancia medida en milímetros
      SerialBT.println(" mm");
  } else {
    SerialBT.println("Fuera de rango");
  }

  delay(100);  // Espera 0.1 segundos entre lecturas para evitar sobrecargar el sistema
}