/********************************************************************

  Por: Ccris01 (ChileCris2011) para el equipo 'Tres absoluto (|3|)'
  Adaptado de 'eeminionn / mustakis2025-2'   

  Auto PID V1.0           Espressif ESP32: V2.0.14 (3.x no funciona)              

********************************************************************/

#include <BluetoothSerial.h>

#include <QTRSensors.h>
#include <Adafruit_VL53L0X.h>
#include <Wire.h>
#include <MPU6050_light.h>

#include "Pines.h"

// PID
float Kp = 0.13;
float Ki = 0.0;
float Kd = 0.5;

int lastError = 0;
int integral = 0;

const int freq = 5000;
const int resolution = 8;


// Velocidad base
const int velocidadBase = 50;

// Sensores QTR
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

// Hitos

// Flags de guardado de dirección en los cuadrados
int primerCuadrado = -1;
int segundoCuadrado = -1;

// Flag para guardar el estado del cuadrado actual, o si no se está en uno
int inCuadrado = -1;

// Configura por donde se evadirá un objeto si es detectado
const int evadirHacia = 0;  // 0 Izquierda | 1 Derecha

// Variable para activar / desactivar la lectura del láser
bool canRead = false;

// Declara la consola serial Bluetooth
BluetoothSerial SerialBT;

// Declara el sensor QTR
QTRSensors qtr;

// Declara el sensor láser (L0X)
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Declara el giroscopio
MPU6050 mpu(Wire);

// ────────────────────────────────────────────────────────────────
void inicializarMotores();
void Motor(int velIzq, int velDer);
// ────────────────────────────────────────────────────────────────

// PID sobre la posición [0..7000] (8 sensores → 7 espacios de 1000)
void PID(uint16_t position) {
  int error = position - 3500;  // centro = 3500
  integral += error;
  int derivative = error - lastError;
  int output = (Kp * error) + (Ki * integral) + (Kd * derivative);
  lastError = error;

  int vIzq = constrain(velocidadBase + output, 0, 255);
  int vDer = constrain(velocidadBase - output, 0, 255);
  Motor(vIzq, vDer);
}

void setup() {
  Serial.begin(115200);

  inicializarMotores();

  pinMode(LED, OUTPUT);
  pinMode(BOTON, INPUT);

  // Config QTR (analógico, mismos pines)
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){ 36, 39, 34, 35, 32, 33, 25, 26 }, SensorCount);
  qtr.setEmitterPin(27);

  if (!lox.begin()) {
    Serial.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1) {
      digitalWrite(LED, HIGH);
      delay(1000);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(500);
    }
  }

  Serial.println(F("VL53L0X iniciado correctamente"));

  //Inicia la conexión I2C (SDA=21 y SCL=22 por defecto; si usas otros pines: 'Wire.begin(SDA, SCL)' )
  Wire.begin();
  delay(200);
  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU init error: ");
    Serial.println(status);
    while (1) {
      digitalWrite(LED, HIGH);
      delay(1500);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(500);
    }
  }

  Serial.println("Calibrando giroscopio. Mantén el robot quieto");

  // Estima offsets del giroscopio
  mpu.calcGyroOffsets();
  Serial.println("Listo.");

  SerialBT.begin("|3|");

  Serial.println("Esperando botón para calibrar");
  while (digitalRead(BOTON) == LOW) delay(10);

  Serial.println("Calibrando sensores...");
  for (uint16_t i = 0; i < 150; i++) {
    qtr.calibrate();
    digitalWrite(LED, HIGH);
    delay(10);
    digitalWrite(LED, LOW);
    delay(10);
  }

  Serial.println("Listo. Esperando para comenzar...");
  while (digitalRead(BOTON) == LOW) delay(10);
  delay(500);
}

void loop() {
  ///////////////////////////////// Obstáculo /////////////////////////////////

  if (canRead) {

    VL53L0X_RangingMeasurementData_t measure;  // Declara la variable para almacenar los datos de la medición

    // Realiza la medición de distancia y almacena los resultados en la variable 'measure'
    lox.rangingTest(&measure, false);

    if (measure.RangeStatus != 4) {
      int lecturaMM = measure.RangeMilliMeter;
      if (lecturaMM < 50) {  // Detecta un objeto (~5cm)
        Motor(-50, -50);                   // Retrocede para no golpear el objeto al girar
        delay(700);
        Motor(0, 0);
        delay(200);
        girar(evadirHacia);  // Gira hacia el lado indicado en 'evadirHacia'
        Motor(50, 50);
        delay(1800);  // Avanza
        Motor(0, 0);
        delay(200);
        girar(evadirHacia + 1);  // Gira hacia el lado contrario (No explicaré como funciona ;] )
        Motor(50, 50);
        delay(3250);  // Avanza
        Motor(0, 0);
        delay(200);
        girar(evadirHacia + 1);  // Gira hacia el lado contrario (Basicamente tiene que ver con la forma en la que se maneja la función)
        while (sensorValues[3] != 0 || sensorValues[4] != 0) {
          qtr.read(sensorValues);  // Avanza hasta detectar la línea
          Motor(50, 50);
        }
        Motor(0, 0);
        delay(200);
        Motor(50, 50);  // Avanza un poco para girar bien
        delay(250);
        qtr.read(sensorValues);
        while (sensorValues[4] != 0) {
          qtr.read(sensorValues);
          girar(evadirHacia);  // Gira hasta acomodarse en la línea
        }
      } else {
        SerialBT.print(lecturaMM);
      }
    } else {
      SerialBT.print("0");
    }
  } else {
    SerialBT.print("-1");
  }
  SerialBT.print("|");
  SerialBT.print((evadirHacia == 1) ? ">" : "<");
  SerialBT.print("||");
  ///////////////////////////////// Hitos /////////////////////////////////

  // Leer crudos
  qtr.read(sensorValues);

  // Umbral: todo lo menor a 3500 lo anulamos
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] < 3500) sensorValues[i] = 0;
  }

  //──────────── Giros 90° | Intersecciones ────────────
  if (sensorValues[0] != 0 && sensorValues[1] != 0 && sensorValues[2] != 0 && sensorValues[3] != 0 && sensorValues[4] != 0 && sensorValues[5] != 0 && sensorValues[6] != 0 && sensorValues[7] != 0) {
    interseccion();
  } else if (sensorValues[0] != 0 && sensorValues[1] != 0 && sensorValues[2] != 0 && sensorValues[3] != 0) {
    giros(0);
  } else if (sensorValues[4] != 0 && sensorValues[5] != 0 && sensorValues[6] != 0 && sensorValues[7] != 0) {
    giros(1);
  }

  ///////////////////////////////// SigueLineas PID /////////////////////////////////

  // Posición ponderada manual
  uint32_t sumaPesada = 0;
  uint32_t sumaTotal = 0;
  for (uint8_t i = 0; i < SensorCount; i++) {
    sumaPesada += (uint32_t)sensorValues[i] * (i * 1000);
    sumaTotal += sensorValues[i];
  }

  // GAP: todo blanco → avanza recto con base y no acumules integral
  if (sumaTotal == 0) {
    Motor(velocidadBase, velocidadBase);
    integral = 0;
    lastError = 0;
    return;
  }

  uint16_t position = sumaPesada / sumaTotal;

  SerialBT.print(position - 3500);

  // Control PID
  PID(position);

  SerialBT.print("||");
  SerialBT.print((primerCuadrado == -1) ? "0" : ((primerCuadrado == 1) ? ">" : "<"));
  SerialBT.print((inCuadrado < 0) ? "0" : "1");
  SerialBT.print((segundoCuadrado == -1) ? "0" : ((segundoCuadrado == 1) ? ">" : "<"));
}
