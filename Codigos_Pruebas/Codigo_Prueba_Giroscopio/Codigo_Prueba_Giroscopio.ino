#include <Wire.h>
#include <MPU6050_light.h>

#define BOTON 12   // opcional: poner a HIGH para poner yaw a 0

MPU6050 mpu(Wire);

// offset dinámico para “poner a cero” solo el yaw sin recalibrar
float yawZero = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(BOTON, INPUT);  // si no usas botón, puedes quitarlo

  // Para ESP32, SDA=21 y SCL=22 por defecto; si usas otros pines: Wire.begin(SDA, SCL);
  Wire.begin();

  delay(200);
  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU init error: ");
    Serial.println(status);
    while (true) { delay(1000); }
  }

  Serial.println("Calibrando giroscopio...");
  // Esta función estima offsets del giroscopio (mantén el sensor quieto durante la calibración)
  mpu.calcGyroOffsets();
  Serial.println("Listo.");
}

void loop() {
  mpu.update();  // ¡Importante! Actualiza la fusión de sensores

  // Zero opcional del yaw (Z) cuando presionas el botón
  if (digitalRead(BOTON) == HIGH) {
    yawZero = mpu.getAngleZ();
  }

  // Lectura de ángulos (en grados)
  float roll  = mpu.getAngleX();
  float pitch = mpu.getAngleY();
  float yaw   = mpu.getAngleZ() - yawZero;  // yaw “reseteado” si usas el botón

  // Imprime cada ~100 ms
  static unsigned long t0 = 0;
  if (millis() - t0 > 100) {
    t0 = millis();
    Serial.print("Roll:\t");  Serial.print(roll, 2);
    Serial.print("\tPitch:\t"); Serial.print(pitch, 2);
    Serial.print("\tYaw:\t");   Serial.println(yaw, 2);
  }
}
