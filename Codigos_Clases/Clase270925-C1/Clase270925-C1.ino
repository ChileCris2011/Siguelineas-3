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
const int velocidadBaseIzq = 70;
const int velocidadBaseDer = 88;

const int baseGiros = 80;

int claser = 0;

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
}

void loop() {
  if (puedeLaser && !blockLaser && lox.isRangeComplete()) {
    int lecturaMM = lox.readRange();
    if (lecturaMM < 100) {
      claser++;
    } else {
      claser = 0;
    }
    if (claser > 5) {  // Detecta un objeto (~10cm)
      Motor(-50, -50);      // Retrocede para no golpear el objeto al girar
      delay(700);
      Motor(0, 0);
      delay(200);
      plusgirar(evadirHacia);  // Gira hacia el lado indicado en 'evadirHacia'
      Motor(50, 50);
      delay(1500);  // Avanza
      Motor(0, 0);
      delay(200);
      girar(evadirHacia + 1);  // Gira hacia el lado contrario (No explicaré como funciona ;] )
      Motor(50, 50);
      delay(4250);  // Avanza
      Motor(0, 0);
      delay(200);
      girar(evadirHacia + 1);  // Gira hacia el lado contrario (Basicamente tiene que ver con la forma en la que se maneja la función)
      qtr.read(sensorValues);
      while (sensorValues[3] < umbral || sensorValues[4] < umbral) {
        qtr.read(sensorValues);  // Avanza hasta detectar la línea
        Motor(50, 50);
      }
      Motor(0, 0);
      delay(200);
      Motor(50, 50);  // Avanza un poco para girar bien
      delay(666);
      qtr.read(sensorValues);
      while (sensorValues[4] < umbral) {
        qtr.read(sensorValues);
        girarCrudo(evadirHacia);  // Gira hasta acomodarse en la línea
      }
      puedeLaser = false;
      blockLaser = true;  // Bloquea el laser para que no siga leyendo
    }
  }

  qtr.read(sensorValues);  // Lectura de los sensores de línea

  // Filtrado simple para el cálculo de posición
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] < 4000) sensorValues[i] = 0;
  }

  // Posición manual ponderada
  uint32_t sumaPesada = 0;
  uint32_t sumaTotal = 0;
  for (uint8_t i = 0; i < SensorCount; i++) {
    sumaPesada += (uint32_t)sensorValues[i] * (i * 1000);
    sumaTotal += sensorValues[i];
  }
  uint16_t position = (sumaTotal > 0) ? (sumaPesada / sumaTotal) : 0;

  // ---- NUEVO: manejo de GAPS (todo blanco) ----
  if (sumaTotal == 0) {
    // Avanza recto con velocidad base para no “morir” en líneas segmentadas
    Motor(velocidadBaseIzq - 10, velocidadBaseDer - 10);
    // Evita acumular integral/derivativos inútiles
    integral = 0;
    lastError = 0;
    // No corremos PID en esta iteración
    return;
  }

  // Disparador de cruce por extremos
  if (sensorValues[0] > 4000 || sensorValues[7] > 4000) evaluarCruce();

  // Seguimiento de línea normal
  PID(position);
}

void evaluarCruce() {

  static int contadorCruce = 0;

  /*
  Ahora se llama directamente a la función evaluarCruce() y se hace la verificación
  dentro para evitar que el PID actúe en los giros, enchuecando el robot
  */

  while (contadorCruce < 3) {
    qtr.read(sensorValues);
    if (sensorValues[0] > 4000 || sensorValues[7] > 4000) contadorCruce++;
    else loop();
  }

  // Umbrales
  const int TH_LADO = 4000;    // extremos (0 y 7)
  const int TH_CENTRO = 3800;  // centrales (2..5) para "hay línea al frente"

  Motor(velocidadBaseIzq - 25, velocidadBaseDer - 25);

  // (3) Avanzar ESCANEANDO para clasificar
  bool vioIzq = false, vioDer = false;
  unsigned long t0 = millis();
  while (millis() - t0 < 200) {
    qtr.read(sensorValues);
    if (sensorValues[0] > TH_LADO) vioIzq = true;
    if (sensorValues[7] > TH_LADO) vioDer = true;
    for (int i = 2; i <= 4; i++) {
      if (sensorValues[i] > TH_CENTRO) {
        break;
      }
    }

    if (vioIzq && vioDer) {
      break;
    }
  }


  qtr.read(sensorValues);

  while (sensorValues[0] > TH_LADO && sensorValues[7] > TH_LADO) {
    qtr.read(sensorValues);
    Motor(velocidadBaseIzq - 30, velocidadBaseDer - 30);
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
  if (vioIzq ^ vioDer) {        // Si SOLO vio UN lado
    if (hayLineaFinal) {        // Si hay línea delante
      if (forzarProximaSemi) {  // Si tiene que forzar la salida
        Motor(40, 40);
        delay(500);
        if (vioIzq) {
          girarCrudo(0);
          delay(700);
          giroWhile(0);
        } else {
          girarCrudo(1);
          delay(700);
          giroWhile(1);
        }
        forzarProximaSemi = false;  // consumir la orden
        // Marcar posicion giroscopio ** <- ???
        //puedeLaser = true;  // Activa la detección (Cambiar segun la ubicacion del obstaculo)
        return;
      }

      // Si NO hay forzado: guardar marca
      if (totalMarcasGuardadas < 2) {  // Si hay menos de 2 marcas guardadas
        if (vioIzq) {
          marcaCuadradoDir[totalMarcasGuardadas] = -1;
        }  // Guarda la marca en el lado que vió
        if (vioDer) {
          marcaCuadradoDir[totalMarcasGuardadas] = +1;
        }
        totalMarcasGuardadas++;
      }

      Motor(40, 40);
      delay(140);
      Motor(0, 0);
      return;  // volver al PID
    } else {
      // SEMI sin línea → giro normal inmediato
      Motor(40, 40);
      delay(500);
      puedeLaser = true;
      if (vioIzq) {
        giroWhile(0);
        return;
      }
      if (vioDer) {
        giroWhile(1);
        return;
      }
    }
  }

  // --- INTERSECCIÓN COMPLETA (ambos lados) ---
  if (vioIzq && vioDer) {
    if (!hayLineaFinal) {              // Si no hay linea delante
      if (totalMarcasGuardadas > 0) {  // Si hay marcas guardadas (Hay cuadrado)
        int dir = marcaCuadradoDir[0];

        // Desplazar las marcas para que la segunda pase a ser primera
        for (int i = 0; i < totalMarcasGuardadas - 1; i++) {
          marcaCuadradoDir[i] = marcaCuadradoDir[i + 1];
        }
        totalMarcasGuardadas--;

        // Ejecutar el giro
        Motor(40, 40);
        delay(500);
        if (dir < 0) giroWhile(0);
        else giroWhile(1);

        forzarProximaSemi = true;  // Forzar la salida (En una semi (90°) con línea delante)
        return;
      } else {  // Si no hay marcas guardadas
        //No hay cuadrado, por lo tanto es el final
        Motor(0, 0);
        delay(500);  // Se detiene
        for (int i = 0; i < 3; i++) {
          digitalWrite(LED, HIGH);  // Juego de luces (3)
          delay(500);
          digitalWrite(LED, LOW);
          delay(500);
        }
        while (true) {}  // fin
      }
    } else {  // Intersección con línea delante
      // Se puede saltar con normalidad (espero...)
      Motor(40, 40);
      delay(140);
      Motor(0, 0);
      return;
    }
  }

  // --- Nada concluyente ---

  /*
  Si el código ha llegado hasta aquí, es que no ha ocurrido nada de lo especificado arriba.
  Aunque es raro...
  ¡No te preocupes, probablemente no sea el robot!
  Sino el programador...
  */

  Motor(0, 0);
  while (true) {}
}
