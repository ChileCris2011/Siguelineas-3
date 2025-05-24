#include <Wire.h>
#include <Adafruit_VL53L0X.h>

// Crea una instancia del sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando...");

  // Inicia la comunicacion I2C con el sensor
  if (!lox.begin()) {
    Serial.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1);  // Detiene el programa si no se puede inicializar el sensor
  }

  Serial.println(F("VL53L0X iniciado correctamente"));
}