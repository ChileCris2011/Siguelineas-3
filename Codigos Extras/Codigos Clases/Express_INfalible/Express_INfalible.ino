#include "pines.h"

#include <QTRSensors.h>
#include <Adafruit_VL53L0X.h>
#include <Wire.h>
#include <MPU6050_light.h>

//variables por entender
bool primerCuadradoIzquierda  = false;
bool primerCuadradoDerecha    = false;
bool segundoCuadradoIzquierda = false;
bool segundoCuadradoDerecha   = false;
int  contadorCasosEspeciales  = 0;
bool huboLineaCentral         = false; // flag general

int  marcaCuadradoDir[2] = {0, 0};  // hasta 2 marcas: -1 izq, +1 der
int  totalMarcasGuardadas = 0;
bool tieneMarcaCuadrado = false;
bool forzarProximaSemi  = false;

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
const int velocidadBase = 80;
const int baseGiros = 50;

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

const int freq = 5000; const int resolution = 8;


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
}

void loop() {
  if (puedeLaser && !blockLaser) {
    verLaser();
  }
  qtr.read(sensorValues);

  // Filtrado simple para el cálculo de posición
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] < 3500) sensorValues[i] = 0;
  }

  // Posición manual ponderada
  uint32_t sumaPesada = 0;
  uint32_t sumaTotal  = 0;
  for (uint8_t i = 0; i < SensorCount; i++) {
    sumaPesada += (uint32_t)sensorValues[i] * (i * 1000);
    sumaTotal  += sensorValues[i];
  }
  uint16_t position = (sumaTotal > 0) ? (sumaPesada / sumaTotal) : 0;

  // ---- NUEVO: manejo de GAPS (todo blanco) ----
  if (sumaTotal == 0) {
    // Avanza recto con velocidad base para no “morir” en líneas segmentadas
    Motor(velocidadBase - 10, velocidadBase - 10);
    // Evita acumular integral/derivativos inútiles
    integral = 0;
    lastError = 0;
    // No corremos PID en esta iteración
    return;
  }

  // Disparador de cruce por extremos
  static int contadorCruce = 0;
  if (sensorValues[0] > 4000 || sensorValues[7] > 4000) contadorCruce++;
  else contadorCruce = 0;

  if (contadorCruce > 3) {
    evaluarCruce();
    contadorCruce = 0;
    return;
  }

  // Seguimiento de línea normal
  PID(position);
}

void evaluarCruce() {
  // Umbrales
  const int TH_LADO   = 4000; // extremos (0 y 7)
  const int TH_CENTRO = 3200; // centrales (2..5) para "hay línea al frente"

  // (2) Retroceder
  Motor(-40, -40);
  delay(300);
  Motor(0, 0);
  delay(100);

  // (3) Avanzar ESCANEANDO para clasificar
  bool vioIzq = false, vioDer = false, vioCentroDuranteScan = false;
  unsigned long t0 = millis();
  while (millis() - t0 < 1000) {
    qtr.read(sensorValues);
    if (sensorValues[0] > TH_LADO) vioIzq = true;
    if (sensorValues[7] > TH_LADO) vioDer = true;
    for (int i = 2; i <= 4; i++) {
      if (sensorValues[i] > TH_CENTRO) {
        vioCentroDuranteScan = true;
        break;
      }
    }
    if (vioIzq && vioDer) {
      Motor(40, 40);   // avance suave de confirmación
      delay(200);
      break;
    }
    Motor(40, 40);   // avance suave de confirmación
    delay(10);
  }

  // (4) Detenerse
  Motor(0, 0);
  delay(100);

  // (5) Revisar si hay línea al frente (lectura estática final)
  qtr.read(sensorValues);
  bool hayLineaFinal = false;
  for (int i = 2; i <= 4; i++) {
    if (sensorValues[i] > TH_CENTRO) {
      hayLineaFinal = true;
      break;
    }
  }

  // (6) Tomar decisión



  // --- SEMI-INTERSECCIÓN (solo un lado) ---
  if (vioIzq ^ vioDer) { // SOLO izquierda o derecha
    puedeLaser = false; // Desactiva la lectura láser
    if (hayLineaFinal) { // Si hay una línea delante
      // Si hay orden pendiente: FORZAR giro en esta semi (según la semi actual)
      if (forzarProximaSemi) {
        forzarNextSemi(vioIzq, vioDer);
        return;
      }

      // Si NO hay forzado: guardar marca (máx. 2)
      guardarMarca(vioIzq, vioDer);

      Motor(40, 40);
      delay(140);
      Motor(0, 0);
      return; // volver al PID
    } else {
      // SEMI sin línea → giro normal inmediato
      girosNoventa(vioIzq, vioDer); // No confundir con giros. Aqui se comprueba donde girar
      return;
    }
  }

  // --- INTERSECCIÓN COMPLETA (ambos lados) ---
  if (vioIzq && vioDer) {
    if (!hayLineaFinal) {
      // COMPLETA y SIN línea: usar próxima marca si existe
      if (totalMarcasGuardadas > 0) {
        entrarCuadrado();
        return;
      } else {
        // Final sin marcas → detener 5 segundos y terminar
        finalizar();
      }
    } else {
      // COMPLETA con línea → comportamiento normal: recto un poco
      Motor(40, 40);
      delay(140);
      Motor(0, 0);
      return;
    }
  }

  // --- Nada concluyente ---
  Motor(0, 0);
  while (true) {}
}
