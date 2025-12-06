#include "Pines.h"

#include <Wire.h>
#include <Adafruit_VL53L0X.h>

#include <BluetoothSerial.h>

// Crea una instancia del sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

BluetoothSerial SerialBT;

const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Project Manhattan");

  Serial.println("Iniciando...");

  inicializarMotores();

  // Configuración Laser
  if (!lox.begin()) {
    SerialBT.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1) {
      digitalWrite(LED, HIGH);
      delay(1000);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(500);
    }
  }
  lox.startRangeContinuous();  // Empieza la lectura contínua del láser

  SerialBT.println(F("VL53L0X iniciado correctamente"));
}

void loop() {

  if (lox.isRangeComplete()) {  // Si el estado es diferente a 4, la medición es válida
    int distancia = lox.readRange();
    SerialBT.print("Distancia: ");
    SerialBT.print(distancia);  // Imprime la distancia medida en centímetros
    SerialBT.println(" mm");
  } else {
    SerialBT.println("Fuera de rango");
  }

  delay(100);  // Espera 0.1 segundos entre lecturas para evitar sobrecargar el sistema
}