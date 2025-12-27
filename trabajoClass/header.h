// Pines motores
#include "Pines.h"

#include <QTRSensors.h>
#include <Adafruit_VL53L0X.h>
#include <Wire.h>
#include <MPU6050_light.h>

#include <BluetoothSerial.h>

#define Izquierda 0
#define Derecha 1

#define null -1

BluetoothSerial SerialBT;

/*
  // Ésto está aquí para poder revisar el código sin necesidad de tener la placa esp32 instalada
  // Define funciones LedC que solo la version 2.x del esp32 contiene
  // Recuerda comentar ésto si lo vas a subir, o descomentarlo si no vas a usar la esp32

  void ledcWrite(uint8_t pin, uint32_t duty) {}
  void ledcSetup(uint8_t chan, uint32_t freq, uint8_t bit_num) {}
  void ledcAttachPin(uint8_t pin, uint8_t chan) {}

*/

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

bool blockLabirint = false;

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
float Kp = 0.18, Ki = 0.0, Kd = 0.5;
int lastError = 0, integral = 0;
int umbral = 4000;
const int velocidadBaseIzq = 105;
const int velocidadBaseDer = 105;

const int delayBase = 200;
const int restaBase = 30;
const int baseGiros = 50;
const int deteccionBase = 50;

const int distEntrance = 1200;

const long vencimiento = 15000;  // 15s

long inicioCuadrado = -1;

int claser = 0;

float target_angle = 0;  // Ángulo objetivo (recto)

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

int contlaser = 0;

float distancia;
bool lab = false;

extern void inicializarMotores();
extern void Motor(int velIzq, int velDer);
extern void girarIzquierda(float grados);
extern void girarDerecha(float grados);
extern void PID(uint16_t position);
extern void guardarMarca();

class Motores_ {
private:
  void izq(int value);
  void der(int value);
  void gDer(float grados);
  void gIzq(float grados);
public:
  void begin(void);
  void drive(int left, int righ);
  void girar.gyro(int direccion, froat grados);
  void girar.undef(int direccion);
  void girar.while (int direccion);
};
extern Motores_ Motores;

class Line_ {
private:
  void PID(uint16_t position);
  void filter_(uint16_t limit);
  uint16_t* user;
  uint16_t valores[sensorCount];
  uint32_t sumaPesada;
  uint32_t sumaTotal;
public:
  void begin(uint16_t* ss);
  void update(void);
  void filter(void);
};
extern Line_ Line;

class Laser_ {
public:
  void begin(void);
};
extern Laser_ Laser;

class Gyro_ {
public:
  void begin(void);
};
extern Gyro_ Gyro