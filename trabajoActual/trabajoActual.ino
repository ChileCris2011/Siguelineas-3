#include "Pines.h"  // "Header"

// Incluir librerìas
#include <QTRSensors.h>
#include <Adafruit_VL53L0X.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include <BluetoothSerial.h>

// Iniciar clase Bluetooth
BluetoothSerial SerialBT;

// Iniciar clase Láser / VL53L0X
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Iniciar clase QTR
QTRSensors qtr;

// Iniciar clase Gyro / Giroscopio / MPU6050
MPU6050 mpu(Wire);

/*
  // Ésto está aquí para poder revisar el código sin necesidad de tener la placa esp32 instalada
  // Define funciones LedC que solo la version 2.x del esp32 contiene
  // Recuerda comentar ésto si lo vas a subir, o descomentarlo si no vas a usar la esp32

  void ledcWrite(uint8_t pin, uint32_t duty) {}
  void ledcSetup(uint8_t chan, uint32_t freq, uint8_t bit_num) {}
  void ledcAttachPin(uint8_t pin, uint8_t chan) {}

*/

// Variabes a usar
int n = 2000; // ;)

//// Cuadrado
int marcaCuadradoDir[2] = { 0, 0 };  // Guarda la dirección de las marcas. Hasta dos al mismo tiempo.
int totalMarcasGuardadas = 0;        // Marcas guardadas al momento

bool forzarProximaSemi = false;  // Forzar proxima semi que vea. Usado para salir del cuadrado.

unsigned long inicioCuadrado = -1;  // Aquí se guarda el tiempo en el que se ingresó al último cuadrado.

//// Laser / Obstaculo
bool puedeLaser = false;  // Activar / desactivar laser. LA DECLARACION INICIAL IMP0RTA.
bool blockLaser = false;  // Como puedeLaser() puede ser activado nuevamente en cualquier parte del código, ésto bloquea todo lo demas.

int evadirHacia = 0;         // Por que lado se debe evadir el obstáculo. Dejar en 0 para cálculo automático.
const bool primer90 = true;  // evadirhacia toma el valor del primer giro de 90. Si está en falso, toma el valor contrario.

int claser = 0;          // Contador laser. Pequeño contador antes de activar el obstáculo para "evitar" falsos positivos
int contlaser = 0;       // Cuantas veces se ha activado.
const int autolock = 2;  // Bloquear automáticamente despues de evadir n veces

const int distance = 100;     // Distancia para evadir el obstáculo
const int acceptclaser = 10;  // Después de cuantas veces activar la evasión

int lecturaMM = -1;  // Lectura del láser. Inicia en -1 porque si.

//// MPU6050 / Giroscopio / Gyro


//// Línea / QTR
const uint8_t SensorCount = 8;       // Número de sensores
uint16_t sensorValues[SensorCount];  // Aquí se guarda el valor de los sensores. El tamaño depende del número de sensores (duh)
const uint16_t umbral = 4000;        // Umbral de cambio entre blanco / negro
int position = 0;                    // Posición respecto a la línea (0-7000)
//// LedC
const int freq = 5000;     // Frecuencia del pulso
const int resolution = 8;  // Resolución. 8 bits = 2^8 = 256 estados (0-255)

//// PID
const float Kp = 0.20, Ki = 0.0, Kd = 0.58;  // Valores Proporcional, Integral y Derivada
int lastError = 0, integral = 0;
bool noPID = false;  // Bloquear PID

//// Motores
const int velBaseIzq = 105, velBaseDer = 105;  // Velocidad base

const int delayBase = 200;  // Normalmente, el tiempo usado para avanzar antes de un giro para acomodarse. Aún hay que hacer algunos calculos para hacer ésto automático. Por mientras, date el trabajo de ajustar ésto ;)

const unsigned long tRetroceso = -1;  // Tiempo que retrocede en la detección
const unsigned long tScan = 150;      // Tiempo de escaneo

void setup() {
  Serial.begin(115200);   // Iniciar comunicación Serial (cable) a 115200 baudios
  SerialBT.begin("|3|");  // Iniciar Serial a través de bluetooth. Nombre: |3|

  pinMode(LED, OUTPUT);
  pinMode(BOTON, INPUT);

  inicializarMotores();  // Configuración de los motores
  inicializarQTR();
  inicializarLaser();

  blink(1, 200);
  inicio();
  inicializarGyro();
  blink(1, 200);
}

void loop() {
  // Láser (Evasión)
  if (puedeLaser && !blockLaser && lox.isRangeComplete()) {
    lecturaMM = lox.readRange();

    if (millis() % 1000 == 0) {  // Imprime cada un segundo para no llenar el búfer (Imprime si millis() es múltiplo de 1000)
      SerialBT.print("Laser: ");
      SerialBT.println(lecturaMM);
    }

    if (lecturaMM < distance) {
      claser++;
    } else {
      claser = 0;
    }

    if (claser > acceptclaser) {
      evadir();
    }
  }
  // Filtrado de valores
  qtr.read(sensorValues);  // Lectura de los sensores de lìnea
  filtrado();

  // Cálculo se posicion
  posicionar();

  //* Gaps
  if (position < 0) {
    // Avanza recto con velocidad base para no “morir” en líneas segmentadas
    Motor(velBaseIzq * 0.75, velBaseDer * 0.75);
    noPID = true;
  }  // (Es solo un if, no lo iba a poner en una función aparte. Para desactivar, elimina el primer '/') */

  // Disparador de cruce por extremos
  if (sensorValues[0] > 4000 || sensorValues[7] > 4000) evaluarCruce();

  if (noPID) {
    // Evita acumular integral/derivativos inútiles
    integral = 0;
    lastError = 0;

    noPID = false;
  } else {
    PID();
  }
}