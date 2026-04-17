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

int marcaCuadradoDir[2] = { 0, 0 };  // hasta 2 marcas: -1 izq, +1 der
int totalMarcasGuardadas = 0;
bool forzarProximaSemi = false;

int lastMark = -1;

bool blockLabirint = false;

bool puedeLaser = true;
bool blockLaser = false;

// Configura por donde se evadirá un objeto si es detectado
const int evadirHacia = 0;  // 0 Izquierda | 1 Derecha


// ----------------- Sensores QTR
const uint8_t SensorCount = 8; // numero de sensores
uint16_t sensorValues[SensorCount];
QTRSensors qtr;

// ----------------- Sensor Laser
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// ----------------- PID (modo normal)
float Kp = 0.15, Ki = 0.0, Kd = 5;
int lastError = 0, integral = 0;

const int umbral = 4000; // umbral de detección de la línea
const int velocidadBaseIzq = 105; // nombres muy descriptivos ;)
const int velocidadBaseDer = 105;

const int delayBase = 200; // delay estándar
const int restaBase = 30; // resta para bajar la velocidad
const int baseGiros = 50; // velocidad base para los giros
const int deteccionBase = 50; // tiempo que detecta los casos especiales una vez que detecta uno

const int distEntrance = 1200; // distancia de entrada al laberinto
const int distObj = 166; // distancia de detección del objeto
const int verDist = 5; // cuantas lecturas seguidas por debajo del umbral para evadir el objeto

const long vencimiento = 15000;  // vencimiento de marcas

long inicioCuadrado = -1;

int claser = 0;

float target_angle = 0;  // Ángulo objetivo (recto) (sin uso)

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

// ----------------- Salida Analógica
const int freq = 5000;
const int resolution = 8;

// ----------------- Láser
int contlaser = 0;

void inicializarMotores();
void Motor(int velIzq, int velDer);
void girarIzquierda(float grados);
void girarDerecha(float grados);
void PID(uint16_t position);
void guardarMarca();

void setup() {
  Serial.begin(115200); // Inicializar consola serial
  SerialBT.begin("|30|"); // Inicializar consola bluetooth
  
  inicializarMotores(); // Inicializar motores

  // Inicializar otros pines
  pinMode(LED, OUTPUT);
  pinMode(BOTON, INPUT);

  calibracionSensores(); // Calibrar sensores

  // Indicación de inicio. Dejar quieto una vez que se prenda la luz!
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(200);

  // Calibración de giroscopio. NO MOVER ROBOT!!!
  calibracionGiroscopio();
}

void loop() {
  // Para validar la lectura: flag del laser activa, laser no bloqueado y laser dentro del rango (no está mirando el aire)
  if (puedeLaser && !blockLaser && lox.isRangeComplete()) {
    int lecturaMM = lox.readRange(); // leer sensor
    SerialBT.println(lecturaMM); // imprimir en consola bluetooth
    
    if (lecturaMM < distObj) { // si esta dentro del rango
      claser++; // aumentar las lecturas seguidas
    } else {
      claser = 0; // si no, resetear
    }
    
    if (claser > verDist) {  // si se cumplen las n lecturas seguidas dentro del rango
      SerialBT.println("Laser..."); // avisar por consola bluetooth la evasión
      Motor(-50, -50);  // Retrocede para no golpear el objeto al girar
      delay(700);
      Motor(0, 0);
      delay(200);
      girar(evadirHacia);  // Gira hacia el lado indicado en 'evadirHacia'
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
      puedeLaser = false; // desactivar laser
      contlaser++; // registrar la evasión
      if (contlaser >= 2) { // Si ya se ha evadido 2 veces
        blockLaser = true;  // Bloquea el laser para que no siga leyendo
      }
    }
  }

  // "quemado" de marcas
  if (inicioCuadrado != -1 && (millis() - inicioCuadrado) >= vencimiento) { // si se ha entrado en un cuadrado, y ya ha pasado mas tiempo que el vencimiento
    forzarProximaSemi = false; // desactivar forzado
    inicioCuadrado = -1; // bajar flag de cuadrado
    SerialBT.println("Marca quemada!"); // avisar por consola bluetooth
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

  // Manejo de gaps (espacios en blanco)
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

/**
 * @brief Evalúa casos especiales
 * @param None
 * @return None
 */
void evaluarCruce() {

  static int contadorCruce = 0; // veces seguidas que se lee un cruce

  // (1) Verificar para evitar falsos positivos

  while (contadorCruce < 3) {
    qtr.read(sensorValues);
    if (sensorValues[0] > 4000 || sensorValues[7] > 4000) {
      contadorCruce++;
    } else {
      while (true) {  // loop
        loop();
      }
    }
  }

  // Umbrales
  const int TH_LADO = 4000;    // extremos (0 y 7)
  const int TH_CENTRO = 4000;  // centrales (2..5) para "hay línea al frente"

  Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);

  // (2) Avanzar ESCANEANDO para clasificar
  bool vioIzq = false, vioDer = false;
  unsigned long t0 = millis();

  while (millis() - t0 < deteccionBase) {
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

  SerialBT.print("vioIzq = ");
  SerialBT.print(vioIzq);
  SerialBT.print("\t vioDer = ");
  SerialBT.print(vioDer);

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

  SerialBT.print("\t Hay Linea = ");
  SerialBT.print(hayLineaFinal);

  // (5) Revisar la ditancia delante (lectura estática final)

  int distLab = 0;

  if (lox.isRangeComplete()) {
    distLab = lox.readRange();
  }

  SerialBT.print("\t distLab = ");
  SerialBT.println(distLab);

  // (6) Tomar decisión

  // --- SEMI-INTERSECCIÓN (solo un lado) ---
  if (vioIzq ^ vioDer) {  // Si SOLO vio UN lado
    if (hayLineaFinal) {  // Si hay línea delante

      if (forzarProximaSemi) {  // Si tiene que forzar la salida
        SerialBT.println("Forzando Semi...");
        Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
        delay(delayBase);
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
        inicioCuadrado = -1;
        // Marcar posicion giroscopio ** <- ???
        //puedeLaser = true;  // Activa la detección (Cambiar segun la ubicacion del obstaculo)
        lastMark = 1;
        return;
      }

      // Si NO hay forzado: guardar marca
      if (totalMarcasGuardadas < 2 && (lastMark == 2 || lastMark == 5)) {  // Si hay menos de 2 marcas guardadas
        SerialBT.println("Guardando Marca...");
        if (vioIzq) {
          marcaCuadradoDir[totalMarcasGuardadas] = -1;
        }  // Guarda la marca en el lado que vió
        if (vioDer) {
          marcaCuadradoDir[totalMarcasGuardadas] = +1;
        }
        totalMarcasGuardadas++;
        lastMark = 2;
      }

      Motor(40, 40);
      delay(140);
      Motor(0, 0);
      return;  // volver al PID
    } else {
      // SEMI sin línea → giro normal inmediato
      SerialBT.println("Giro 90...");
      Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
      delay(delayBase);
      puedeLaser = true;
      if (vioIzq) {
        girarCrudo(0);
        delay(333);
        giroWhile(0);
        Motor(0, 0);
        delay(delayBase);
        lastMark = 3;
        return;
      }
      if (vioDer) {
        girarCrudo(1);
        delay(333);
        giroWhile(1);
        Motor(0, 0);
        delay(delayBase);
        lastMark = 3;
        return;
      }
    }
  }

  // --- INTERSECCIÓN COMPLETA (ambos lados) ---
  if (vioIzq && vioDer) {
    if (!hayLineaFinal) {  // Si no hay linea delante

      /*
        if (distLab < distEntrance && !blockLabirint) {
        SerialBT.println("Laberinto...");
        Motor(velocidadBaseIzq - (velocidadBaseIzq - 10), velocidadBaseDer - (velocidadBaseDer - 10));
        delay(delayBase);
        labirint();
        digitalWrite(LED, LOW);
        return;
        }
      */

      if (totalMarcasGuardadas > 0) {  // Si hay marcas guardadas (Hay cuadrado)
        SerialBT.println("Cuadrado...");
        int dir = marcaCuadradoDir[0];

        // Desplazar las marcas para que la segunda pase a ser primera
        for (int i = 0; i < totalMarcasGuardadas - 1; i++) {
          marcaCuadradoDir[i] = marcaCuadradoDir[i + 1];
        }
        totalMarcasGuardadas--;

        // Ejecutar el giro
        Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
        delay(delayBase);
        if (dir < 0) giroWhile(0);
        else giroWhile(1);

        forzarProximaSemi = true;  // Forzar la salida (En una semi (90°) con línea delante)
        inicioCuadrado = millis();
        lastMark = 4;
        return;
      } else {  // Si no hay marcas guardadas
        //No hay cuadrado, por lo tanto es el final
        SerialBT.println("¡Final!");
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
      SerialBT.println("Cruce...");
      Motor(40, 40);
      delay(140);
      Motor(0, 0);
      lastMark = 5;
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

  SerialBT.println("Nada concluyente...");

  Motor(velocidadBaseIzq - (velocidadBaseIzq - 10), velocidadBaseDer - (velocidadBaseDer - 10));
}
