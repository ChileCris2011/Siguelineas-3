#include <QTRSensors.h>
#include <Adafruit_VL53L0X.h>
#include <Wire.h>
#include <MPU6050_light.h>

#include <BluetoothSerial.h>

// --- Cuadrado ---
int marcas[2] = { 0, 0 };
int marcasGuardadas = 0;
bool enCuadrado = false;

// --- Laser ---
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

bool obstaculo = true;
const int evadirHacia = 0;
const int distanciaObstaculo = 222;

// --- QTR ---
const uint8_t sensorCount = 8;
const uint8_t pinesQTR[sensorCount] = { 36, 39, 34, 35, 32, 33, 25, 26 };
uint16_t sensorValues[sensorCount];
const int umbral = 4000;
QTRSensors qtr;

// --- PID ---
const float Kp = 0.25, Ki = 0.0, Kd = 1;
float lastError = 0, integral = 0;

// --- Motores ---
#define AIN1 16
#define AIN2 17
#define PWMA 4
#define BIN1 5
#define BIN2 18
#define PWMB 19

const int velBaseIzq = 95;
const int velBaseDer = 100;
const int tiempoEscaneo = 120;

// --- Giroscopio ---
MPU6050 mpu(Wire);

// --- Otros ---
const int freq = 5000;
const int resolution = 8;

#define BOTON 12

#define LED 2