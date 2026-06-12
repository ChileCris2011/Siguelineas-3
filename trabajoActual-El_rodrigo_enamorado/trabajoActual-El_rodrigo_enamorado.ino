/************************************************************************
*                                                                       *
*    SigueLineas |3|, 2026. Por el equipo |30| (treinta absoluto)       *
*                                                                       *
*    Para nuestros compañeros de clases y los demás equipos.            *
*                                                                       *
*    Recoleta, 2026. Licencia MIT (c) 2025 Ccris                        *
*                                                                       *
************************************************************************/

// Pines motores
#include "Pines.h"

#include <QTRSensors.h>
#include <Adafruit_VL53L0X.h>
#include <Wire.h>
#include <MPU6050_light.h>

#include <BluetoothSerial.h>

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

int marcaCuadradoDir[2] = { 0, 0 };  // hasta 2 marcas: -1 izq, +1 der
int totalMarcasGuardadas = 0;
bool forzarProximaSemi = false;

int lastMark = 0;

bool blockLabirint = false;

bool puedeLaser = true;
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
float Kp = 0.25, Ki = 0.0, Kd = 0.8;
int lastError = 0, integral = 0;
int umbral = 4000;
const int velocidadBaseIzq = 130;
const int velocidadBaseDer = 130;

const int delayBase = 144;
const int restaBase = 30;
const int baseGiros = 100;

const int distEntrance = 1200;

const unsigned long vencimiento = 1500;
unsigned long iniciomarca = -1;
int prevmark = 0;

int claser = 0;

float target_angle = 0;  // Ángulo objetivo (recto)

// ----------------- Tiempos
const unsigned long tRetroceso = 0;
const unsigned long tScan = 50;
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


void inicializarMotores();
void Motor(int velIzq, int velDer);
int girarIzquierda(float grados, bool read = false);
int girarDerecha(float grados, bool read = false);
void PID(uint16_t position);
void guardarMarca();
void updateLog(int actstat = 0);
void elog(String message, bool st = true, bool ln = true);
void errorLog(String message, bool st = true, bool ln = true);
void elog(int message, bool st = true, bool ln = true);
void errorLog(int message, bool st = true, bool ln = true);

void setup() {
  Serial.begin(115200);
  SerialBT.begin("|3| 2");
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

int lecturaMM = -1;

void loop() {
  if (puedeLaser && !blockLaser && lox.isRangeComplete()) {
    lecturaMM = lox.readRange();
    elog(lecturaMM);
    if (lecturaMM < 222) {
      claser++;
    } else {
      claser = 0;
    }
    if (claser > 5) {  // Detecta un objeto (~10cm)
      elog("Laser...");
      updateLog(4);
      Motor(-50, -50);  // Retrocede para no golpear el objeto al girar
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
      delay(3750);  // Avanza
      Motor(0, 0);
      delay(200);
      girar(evadirHacia + 1);  // Gira hacia el lado contrario (Basicamente tiene que ver con la forma en la que se maneja la función)
      qtr.read(sensorValues);
      while (sensorValues[3] < umbral || sensorValues[4] < umbral) {
        updateLog(4);
        qtr.read(sensorValues);  // Avanza hasta detectar la línea
        Motor(50, 50);
      }
      Motor(0, 0);
      delay(200);
      Motor(50, 50);  // Avanza un poco para girar bien
      delay(666);
      qtr.read(sensorValues);
      while (sensorValues[4] < umbral) {
        updateLog(4);
        qtr.read(sensorValues);
        girarCrudo(evadirHacia);  // Gira hasta acomodarse en la línea
      }
      puedeLaser = false;
      blockLaser = true;
    }
  } else {
    lecturaMM = -1;  // No se está leyendo el sensor
  }

  if ((millis() - iniciomarca) > vencimiento && iniciomarca != -1) {
    elog("Marca quemada...");
    iniciomarca = -1;
    lastMark = 5;
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

  qtr.read(sensorValues);

  bool hasgap = true;

  for (int i = 0; i < SensorCount; i++) {
    if (sensorValues[i] > umbral) {
      hasgap = false;
      break;
    }
  }

  if (hasgap && !forzarProximaSemi) {
    integral = 0;
    lastError = 0;
    Motor(0, 0);
    delay(200);
    gaps();
    return;
  }

  // Disparador de cruce por extremos
  if (sensorValues[0] > 4000 || sensorValues[7] > 4000) evaluarCruce();

  updateLog(forzarProximaSemi ? 1 : 0);

  // Seguimiento de línea normal
  PID(position);
}

void evaluarCruce() {

  static int contadorCruce = 0;

  /*
    Ahora se llama directamente a la función evaluarCruce() y se hace la verificación
    dentro para evitar que el PID actúe en los giros, enchuecando el robot
  */

  // (0) Verificar para evitar falsos positivos

  while (contadorCruce < 3) {
    qtr.read(sensorValues);
    if (sensorValues[0] > 4000 || sensorValues[7] > 4000) {
      contadorCruce++;
    } else {
      while (true) {  // para seguir con la logica de bucle de loop
        loop();
      }
    }
  }

  SerialBT.println("=");

  // Umbrales
  const int TH_LADO = 4000;    // extremos (0 y 7)
  const int TH_CENTRO = 4000;  // centrales (2..5) para "hay línea al frente"

  // (1) Retroceder para iniciar el escaneo (Descontinuado / Mantenido por compatibilidad con Pasito)

  Motor(velocidadBaseIzq, velocidadBaseDer);
  delay(tRetroceso);

  Motor(0, 0);
  delay(200);

  // (2) Avanzar ESCANEANDO para clasificar
  bool vioIzq = false, vioDer = false;
  tInicioScan = millis();

  Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);

  while (millis() - tInicioScan < tScan) {
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
    Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
  }

  delay(55);

  // (3) Detenerse
  Motor(0, 0);

  // (4) Revisar si hay línea al frente (primera lectura estática)
  qtr.read(sensorValues);
  bool hayLineaFinal = false;
  for (int i = 2; i <= 4; i++) {
    if (sensorValues[i] > TH_CENTRO) {
      hayLineaFinal = true;
      break;
    }
  }

  // (5) Revisar la distancia delante (lectura estática final)

  int distLab = 0;

  if (lox.isRangeComplete()) {
    distLab = lox.readRange();
  }

  SerialBT.print(hayLineaFinal ? "1" : "0");
  SerialBT.print("|");
  SerialBT.print(vioIzq ? "1" : "0");
  SerialBT.print("|");
  SerialBT.print(vioDer ? "1" : "0");
  SerialBT.print("|");
  SerialBT.println(distLab);

  delay(200);

  SerialBT.println("=");

  delay(200);

  elog("vioIzq = ", true, false);
  elog(vioIzq, false, false);
  elog("\t vioDer = ", false, false);
  elog(vioDer, false, false);
  elog("\t Hay Linea = ", false, false);
  elog(hayLineaFinal, false, false);
  elog("\t distLab = ", false, false);
  elog(distLab, false);

  // (6) Tomar decisión

  // --- SEMI-INTERSECCIÓN (solo un lado) ---
  if (vioIzq ^ vioDer) {  // Si SOLO vio UN lado
    if (hayLineaFinal) {  // Si hay línea delante

      if (forzarProximaSemi) {  // Si tiene que forzar la salida
        elog("Forzando Semi...");
        Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
        delay(delayBase);
        if (vioIzq) {
          giroSal(0);
          Motor(0, 0);
          delay(200);
          giroWhile(0);
        } else {
          giroSal(1);
          Motor(0, 0);
          delay(200);
          giroWhile(1);
        }
        forzarProximaSemi = false;  // consumir la orden
        // Marcar posicion giroscopio ** <- ???
        //puedeLaser = true;  // Activa la detección (Cambiar segun la ubicacion del obstaculo)
        lastMark = 1;
        return;
      }

      // Si NO hay forzado: guardar marca
      if (totalMarcasGuardadas < 2 && (lastMark > 20 || lastMark == 7)) {  // Si hay menos de 2 marcas guardadas y el anterior fue cruce o marca guardada
        elog("Evaluando marca | ", true, false);
        if (iniciomarca > 0) {  // Si es que todavía estamos dentro del tiempo para una segunda marca
          iniciomarca = -1;

          marcaCuadradoDir[totalMarcasGuardadas] = prevmark;
          totalMarcasGuardadas++;

          if (vioIzq) {
            marcaCuadradoDir[totalMarcasGuardadas] = -1;
            lastMark = (prevmark = 1) ? 32 : 22;
            elog("Guardada!: ", false, false);
            elog((prevmark = 1) ? "Der | Izq" : "Izq | Izq");
          }  // Guarda la marca en el lado que vió
          if (vioDer) {
            marcaCuadradoDir[totalMarcasGuardadas] = 1;
            lastMark = (prevmark = 1) ? 33 : 23;
            elog("Guardada!: ", false, false);
            elog((prevmark = 1) ? "Der | Der" : "Izq | Der");
          }
          prevmark = 0;
          totalMarcasGuardadas++;
        } else {  // Si el temporizador no ha sido activado (no hubo primera marca)
          if (vioIzq) {
            prevmark = -1;
            elog("Preliminar Izq. ", false, false);
          }  // Memoriza la primera marca en el lado que vió, sin guardarla aún
          if (vioDer) {
            prevmark = 1;
            elog("Preliminar Der. ", false, false);
          }
          totalMarcasGuardadas++;
          iniciomarca = millis();
        }
      } else if (lastMark < 20 && lastMark != 3 && lastMark != 7) {
        elog("Marca sin condicion", false, false);
        lastMark = 4;
      }

      elog("...", false);

      Motor(0, 0);
      delay(144);
      return;  // volver al PID
    } else {
      // SEMI sin línea → giro normal inmediato
      elog("Giro 90...");
      Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
      delay(delayBase);
      // puedeLaser = true;
      if (vioIzq) {
        giroSal(0);
        Motor(0, 0);
        delay(200);
        giroWhile(0);
        lastMark = 6;
        return;
      }
      if (vioDer) {
        giroSal(1);
        Motor(0, 0);
        delay(200);
        giroWhile(1);
        lastMark = 6;
        return;
      }
    }
  }

  // --- INTERSECCIÓN COMPLETA (ambos lados) ---
  if (vioIzq && vioDer) {
    if (!hayLineaFinal) {  // Si no hay linea delante

      /*
        if (distLab < distEntrance && !blockLabirint) {
        elog("Laberinto...");
        Motor(velocidadBaseIzq - (velocidadBaseIzq - 10), velocidadBaseDer - (velocidadBaseDer - 10));
        delay(delayBase);
        labirint();
        digitalWrite(LED, LOW);
        return;
        }
      */

      if (totalMarcasGuardadas > 0) {  // Si hay marcas guardadas (Hay cuadrado)
        elog("Cuadrado ");
        int dir = marcaCuadradoDir[0];

        elog(" (", true, false);
        elog(dir, false, false);
        elog(", ", false, false);
        elog((dir == 1) ? "Der)..." : "Izq)...", false);

        // Desplazar las marcas para que la segunda pase a ser primera
        for (int i = 0; i < totalMarcasGuardadas - 1; i++) {
          marcaCuadradoDir[i] = marcaCuadradoDir[i + 1];
        }
        totalMarcasGuardadas--;

        // Ejecutar el giro
        Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
        delay(delayBase);
        if (dir == 1) giroWhile(1);
        else giroWhile(0);

        forzarProximaSemi = true;  // Forzar la salida (En una semi (90°) con línea delante)
        return;
      } else {  // Si no hay marcas guardadas
        //No hay cuadrado, por lo tanto es el final
        elog("¡Final!");
        Motor(0, 0);
        delay(500);  // Se detiene
        for (int i = 0; i < 3; i++) {
          digitalWrite(LED, HIGH);  // Juego de luces (3)
          delay(500);
          digitalWrite(LED, LOW);
          delay(500);
        }
        SerialBT.println("4095|4095|4095|4095|4095|4095|4095|4095/-1/0/0|0/0");
        while (true) {}  // fin
      }
    } else {  // Intersección con línea delante
      // Se puede saltar con normalidad (espero...)
      elog("Cruce...");
      Motor(40, 40);
      delay(140);
      Motor(0, 0);
      lastMark = 7;
      return;
    }
  }

  // --- Nada concluyente ---

  /*
    Si el código ha llegado hasta aquí, es que no ha ocurrido nada de lo especificado arriba.
    Aunque es raro...
    ¡No te preocupes, probablemente no sea el robot!
    Sino el programador...

    Okay, es mentira. La mayoría de veces es solo un rayo cósmico... tal vez...
  */

  elog("Nada conluyente...");

  Motor(velocidadBaseIzq - (velocidadBaseIzq - 10), velocidadBaseDer - (velocidadBaseDer - 10));
  lastMark = -1;
}
