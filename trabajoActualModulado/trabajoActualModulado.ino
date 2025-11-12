// Pines motores
#include "Pines.h"

#include <QTRSensors.h>
#include <Adafruit_VL53L0X.h>
#include <Wire.h>
#include <MPU6050_light.h>


/*
// Ésto está aquí para poder revisar el código sin necesidad de tener la placa esp32 instalada
// Define funciones LedC que solo la version 2.x del esp32 contiene
// Recuerda comentar ésto si lo vas a subir, o descomentarlo si no vas a usar la esp32

void ledcWrite(uint8_t pin, uint32_t duty) {}
void ledcSetup(uint8_t chan, uint32_t freq, uint8_t bit_num) {}
void ledcAttachPin(uint8_t pin, uint8_t chan) {}

*/


uint32_t sumaPesada = 0;
uint32_t sumaTotal = 0;

int claser = 0;

//variables por entender
bool primerCuadradoIzquierda = false;
bool primerCuadradoDerecha = false;
bool segundoCuadradoIzquierda = false;
bool segundoCuadradoDerecha = false;
int contadorCasosEspeciales = 0;
bool huboLineaCentral = false;  // flag general

int marcaCuadradoDir[2] = { 0, 0 };  // hasta 2 marcas: -1 izq, +1 der
int totalMarcasGuardadas = 0;
bool tieneMarcaCuadrado = false;
bool forzarProximaSemi = false;

bool puedeLaser = false;
bool blockLaser = false;

// Configura por donde se evadirá un objeto si es detectado
const int evadirHacia = 0;  // 0 Izquierda | 1 Derecha


// ----------------- Sensores QTR
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
QTRSensors qtr;

// ----------------- Sensor Laser
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// ----------------- PID (modo normal)
float Kp = 0.15, Ki = 0.0, Kd = 0.5;
int lastError = 0, integral = 0;
int umbral = 4000;
const int velocidadBaseIzq = 80;
const int velocidadBaseDer = 100;
const int delayGiro = 250;

const int minusGiro = 25;

const int baseGiros = 100;

// ----------------- Estados
bool escaneando = false;
bool flagIzquierda = false, flagDerecha = false;
int filtroBordeCount = 0;

// ----------------- Tiempos
const unsigned long tRetroceso = 350;
const unsigned long tScan = 1000;
unsigned long tInicioScan = 0;

// ----------------- MPU6050
MPU6050 mpu(Wire);
float yawZero = 0.0;

// ----------------- Flags de marca
bool marcaPrimeraIzq = false;
bool marcaPrimeraDer = false;
bool marcaGuardada = false;  // indica si ya se guardó la primera marca

// ----------------- Prototipos

const int freq = 5000;
const int resolution = 8;


void inicializarMotores();
void Motor(int velIzq, int velDer);
void girarIzquierda(float grados);
void girarDerecha(float grados);
void PID(uint16_t position);
void guardarMarca();

void setup() {
  Serial.begin(115200);
  inicializarMotores();
  pinMode(LED, OUTPUT);
  pinMode(BOTON, INPUT);

  calibracionSensores();
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(200);
  calibracionGiroscopio();
  iniciarLaser();
}

void loop() {
  leerLaser();

  qtr.read(sensorValues);  // Lectura de los sensores de línea

  uint16_t position = posicion();

  // ---- NUEVO: manejo de GAPS (todo blanco) ----
  gaps();

  // Disparador de cruce por extremos
  if (sensorValues[0] > 4000 || sensorValues[7] > 4000) evaluarCruce();

  // Seguimiento de línea normal
  PID(position);
}