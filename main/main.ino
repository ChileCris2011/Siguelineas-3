/*******************************************************************
*                                                                  *
*    SigueLineas |3|, 2026. Por el equipo |3| (tres absoluto)      *
*                                                                  *
*    Para nuestros queridos compañeros                             *
*    (cualquier cosa, ya saben a quien preguntar ;)                *
*                                                                  *
*    Recoleta, 2026. Licencia MIT (c) 2025 Ccris                   *
*                                                                  *
*******************************************************************/

// Pines motores
#include "Pines.h"

#include <QTRSensors.h>
#include <Adafruit_VL53L0X.h>
#include <Wire.h>
#include <MPU6050_light.h>

#include <Adafruit_TCS34725.h>

#include <BluetoothSerial.h>

// Umbrales de color calibrados en pista con el codigo de prueba estatico.
// Se usan valores crudos porque la decision se ajusta manualmente durante las pruebas.

bool detectarMarcadoresVerdes = true; // false si no harás los cuadrados

int verdeMaximo = 185;
int verdeMinimo = 100;

// En rampa aparecieron falsos positivos con rojo alto; un verde real queda bajo este valor.
int rojoMaximoParaVerde = 100;

BluetoothSerial SerialBT;

// Direcciones guardadas para el desafio del cuadrado: -1 izquierda, +1 derecha.
int direccionesCuadradoGuardadas[2] = { 0, 0 };
int totalDireccionesCuadrado = 0;
bool forzarSalidaCuadradoEnSemi = false;

// Ultimo evento reconocido. Se usa para distinguir marcas del cuadrado de cruces normales.
const int EVENTO_NINGUNO = -1;
const int EVENTO_SALIDA_CUADRADO = 1;
const int EVENTO_MARCA_CUADRADO = 2;
const int EVENTO_GIRO_90 = 3;
const int EVENTO_ENTRADA_CUADRADO = 4;
const int EVENTO_CRUCE_RECTO = 5;
int ultimoEventoPista = EVENTO_NINGUNO;

bool laberintoYaResuelto = false;

bool deteccionObstaculoHabilitada = true;
bool obstaculoYaEvadido = false;

// Configura por donde se evadirá un objeto si es detectado
const int evadirHacia = 0;  // 0 Izquierda | 1 Derecha

// ----------------- Sensores QTR
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
QTRSensors qtr;

// ----------------- Sensor Laser
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// ----------------- PID (modo normal)
float Kp = 0.25, Ki = 0.0, Kd = 1;
float lastError = 0, integral = 0;
int umbral = 4000;
const int velocidadBaseIzq = 95;
const int velocidadBaseDer = 100;

const int delayBase = 145;
const int restaBase = 40;
const int baseGiros = 100;
const int deteccionBase = 120;

const int distanciaEntradaLaberintoMm = 1200;

int lecturasObjetoConsecutivas = 0;

// ----------------- MPU6050
MPU6050 mpu(Wire);

// ----------------- Color
#define MUXADDR 0x70

// Canales del multiplexor
#define CANAL_SENSOR_1 0
#define CANAL_SENSOR_2 1

// Ambos sensores usan la misma dirección I2C (0x29),
// pero al estar en canales distintos del mux no hay conflicto.
Adafruit_TCS34725 rgb1 = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_1X);
Adafruit_TCS34725 rgb2 = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_101MS, TCS34725_GAIN_1X);

// ----------------- Prototipos

const int freq = 5000;
const int resolution = 8;

void inicializarMotores();
void Motor(int velIzq, int velDer);
int girarIzquierda(float grados, bool detenerAlEncontrarLinea = false);
int girarDerecha(float grados, bool detenerAlEncontrarLinea = false);
void PID(uint16_t position);

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

void loop() {
  if (deteccionObstaculoHabilitada && !obstaculoYaEvadido && lox.isRangeComplete()) {
    int lecturaMM = lox.readRange();
    SerialBT.println(lecturaMM);
    if (lecturaMM < 222) {
      lecturasObjetoConsecutivas++;
    } else {
      lecturasObjetoConsecutivas = 0;
    }
    if (lecturasObjetoConsecutivas > 5) {  // Confirma varias lecturas para evitar falsos positivos.
      SerialBT.println("Laser...");
      Motor(-50, -50);  // Retrocede para no golpear el objeto al girar
      delay(700);
      Motor(0, 0);
      delay(200);
      plusgirar(evadirHacia);  // Gira hacia el lado indicado en 'evadirHacia'
      Motor(50, 50);
      delay(1500);  // Avanza
      Motor(0, 0);
      delay(200);
      girar(evadirHacia + 1);  // Gira hacia el lado contrario para rodear el cubo.
      Motor(50, 50);
      delay(3750);  // Avanza
      Motor(0, 0);
      delay(200);
      girar(evadirHacia + 1);  // Reorienta el robot para buscar nuevamente la linea.
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
      deteccionObstaculoHabilitada = false;
      obstaculoYaEvadido = true;
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

  // ---- Manejo de GAPS (todo blanco) ----

  qtr.read(sensorValues);

  bool hayGap = true;

  for (int i = 0; i < SensorCount; i++) {
    if (sensorValues[i] > umbral) {
      hayGap = false;
      break;
    }
  }

  if (hayGap && !forzarSalidaCuadradoEnSemi) {
    integral = 0;
    lastError = 0;
    Motor(0, 0);
    delay(200);
    gaps();
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

  // (1) Verificar para evitar falsos positivos

  while (contadorCruce < 3) {
    qtr.read(sensorValues);
    if (sensorValues[0] > 4000 || sensorValues[7] > 4000) {
      contadorCruce++;
    } else {
      contadorCruce = 0;
      return;
    }
  }

  delay(20);
  Motor(0, 0);
  delay(200);

  // (2) Leer cuadrados verdes

  bool rverde = false;
  bool lverde = false;

  if (detectarMarcadoresVerdes) {

    // Revisar el color debajo
    uint16_t r1, g1, b1, c1;
    muxSelect(CANAL_SENSOR_1);
    rgb1.getRawData(&r1, &g1, &b1, &c1);

    delay(200);

    uint16_t r2, g2, b2, c2;
    muxSelect(CANAL_SENSOR_2);
    rgb2.getRawData(&r2, &g2, &b2, &c2);

    if (g1 > verdeMinimo && g1 < verdeMaximo && r1 < rojoMaximoParaVerde) {
      digitalWrite(LED, HIGH);
      rverde = true;
    }

    if (g2 > verdeMinimo && g2 < verdeMaximo && r2 < rojoMaximoParaVerde) {
      digitalWrite(LED, HIGH);
      lverde = true;
    }

    SerialBT.print("| Color: : ");
    SerialBT.print(r1);
    SerialBT.print(", ");
    SerialBT.print(g1);
    SerialBT.print(", ");
    SerialBT.print(b1);
    SerialBT.print(", ");
    SerialBT.print(c1);
    SerialBT.print(" | ");

    SerialBT.print(r2);
    SerialBT.print(", ");
    SerialBT.print(g2);
    SerialBT.print(", ");
    SerialBT.print(b2);
    SerialBT.print(", ");
    SerialBT.print(c2);
    SerialBT.print(" | ");
  }

  // Umbrales
  const int TH_LADO = 4000;    // extremos (0 y 7)
  const int TH_CENTRO = 4000;  // centrales (2..5) para "hay línea al frente"

  // (3) Avanzar ESCANEANDO para clasificar

  Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);

  bool vioIzq = false, vioDer = false;
  unsigned long t0 = millis();

  int mDer = 0;
  int mIzq = 0;

  while (millis() - t0 < deteccionBase) {
    qtr.read(sensorValues);

    if (sensorValues[0] > mIzq) mIzq = sensorValues[0];
    if (sensorValues[7] > mDer) mDer = sensorValues[7];

    if (sensorValues[0] > TH_LADO) vioIzq = true;
    if (sensorValues[7] > TH_LADO) vioDer = true;

    if (vioIzq && vioDer) {
      break;
    }
  }

  SerialBT.print("vioIzq = ");
  SerialBT.print(vioIzq);
  SerialBT.print("(");
  SerialBT.print(mIzq);
  SerialBT.print(")\t vioDer = ");
  SerialBT.print(vioDer);
  SerialBT.print("(");
  SerialBT.print(mDer);
  SerialBT.print(")");

  // (4) Avanzar hasta blanco

  qtr.read(sensorValues);

  while (sensorValues[0] > TH_LADO || sensorValues[7] > TH_LADO) {
    qtr.read(sensorValues);
    Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
  }

  delay(55);

  // (5) Detenerse
  Motor(0, 0);

  // (6) Revisar si hay línea al frente (primera lectura estática)
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

  /*/ (7) Revisar la distancia delante (lectura estática final)

  int distLab = 0;

  if (lox.isRangeComplete()) {
    distLab = lox.readRange();
  }

  SerialBT.print("\t distLab = ");
  SerialBT.print(distLab);
  //*/

  SerialBT.print("\n");

  // (8) Tomar decisión

  // --- SEMI-INTERSECCIÓN (solo un lado) ---
  if (vioIzq ^ vioDer) {  // Si SOLO vio UN lado
    if (hayLineaFinal) {  // Si hay línea delante

      if (forzarSalidaCuadradoEnSemi) {  // Si viene saliendo del cuadrado, debe tomar esta semi-interseccion.
        SerialBT.println("Forzando Semi...");
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
        forzarSalidaCuadradoEnSemi = false;  // consumir la orden
        //deteccionObstaculoHabilitada = true;  // Activa la detección (Cambiar segun la ubicacion del obstaculo)
        ultimoEventoPista = EVENTO_SALIDA_CUADRADO;
        return;
      }

      // Cuadrados verdes

      if (lverde) {
        SerialBT.println("VERDE izq");
        giroSal(0);
        Motor(0, 0);
        delay(200);
        giroWhile(0);
        Motor(0, 0);
        digitalWrite(LED, LOW);
        return;
      }

      if (rverde) {
        SerialBT.println("VERDE der");
        giroSal(1);
        Motor(0, 0);
        delay(200);
        giroWhile(1);
        Motor(0, 0);
        digitalWrite(LED, LOW);
        return;
      }

      // Si NO hay forzado: guardar marca
      if (totalDireccionesCuadrado < 2 && (ultimoEventoPista == EVENTO_MARCA_CUADRADO || ultimoEventoPista == EVENTO_CRUCE_RECTO)) {
        SerialBT.print("Guardando Marca");
        if (vioIzq) {
          direccionesCuadradoGuardadas[totalDireccionesCuadrado] = -1;
          SerialBT.print(" (-1, Izq)");
        }  // Guarda la marca en el lado que vió
        if (vioDer) {
          direccionesCuadradoGuardadas[totalDireccionesCuadrado] = 1;
          SerialBT.print(" (+1, Der)");
        }
        totalDireccionesCuadrado++;
        ultimoEventoPista = EVENTO_MARCA_CUADRADO;
      } else if (ultimoEventoPista != EVENTO_MARCA_CUADRADO && ultimoEventoPista != EVENTO_CRUCE_RECTO) {
        SerialBT.print("Marca sin condicion");
      }
      SerialBT.println("...");

      Motor(0, 0);
      delay(144);
      return;  // volver al PID
    } else {
      // SEMI sin línea → giro normal inmediato
      SerialBT.println("Giro 90...");
      Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
      delay(delayBase);
      // deteccionObstaculoHabilitada = true;
      if (vioIzq) {
        giroSal(0);
        Motor(0, 0);
        delay(200);
        giroWhile(0);
        ultimoEventoPista = EVENTO_GIRO_90;
        lastError = 0;
        integral = 0;
        return;
      }
      if (vioDer) {
        giroSal(1);
        Motor(0, 0);
        delay(200);
        giroWhile(1);
        ultimoEventoPista = EVENTO_GIRO_90;
        lastError = 0;
        integral = 0;
        return;
      }
    }
  }

  // --- INTERSECCIÓN COMPLETA (ambos lados) ---
  if (vioIzq && vioDer) {

    // Cuadrados verdes

    if (lverde && rverde) {
      SerialBT.println("VERDE 180");
      digitalWrite(LED, LOW);
      delay(200);
      digitalWrite(LED, HIGH);
      delay(200);
      Motor(-velocidadBaseIzq, -velocidadBaseDer);
      delay(500);
      Motor(0, 0);
      giroSal(evadirHacia);
      Motor(0, 0);
      delay(200);
      giroWhile(evadirHacia);
      Motor(0, 0);
      delay(500);
      digitalWrite(LED, LOW);
      return;
    }

    if (lverde) {
      SerialBT.println("VERDE izq");
      giroSal(0);
      Motor(0, 0);
      delay(200);
      giroWhile(0);
      Motor(0, 0);
      digitalWrite(LED, LOW);
      return;
    }

    if (rverde) {
      SerialBT.println("VERDE der");
      giroSal(1);
      Motor(0, 0);
      delay(200);
      giroWhile(1);
      Motor(0, 0);
      digitalWrite(LED, LOW);
      return;
    }

    if (!hayLineaFinal) {  // Si no hay linea delante

      /*
        if (distLab < distanciaEntradaLaberintoMm && !laberintoYaResuelto) {
        SerialBT.println("Laberinto...");
        Motor(velocidadBaseIzq - (velocidadBaseIzq - 10), velocidadBaseDer - (velocidadBaseDer - 10));
        delay(delayBase);
        labirint();
        digitalWrite(LED, LOW);
        return;
        }
      */

      if (totalDireccionesCuadrado > 0) {  // Si hay marcas guardadas (Hay cuadrado)
        SerialBT.print("Cuadrado ");
        int dir = direccionesCuadradoGuardadas[0];

        SerialBT.print(" (");
        SerialBT.print(dir);
        SerialBT.print(", ");
        SerialBT.println((dir == 1) ? "Der)..." : "Izq)...");

        // Desplazar las marcas para que la segunda pase a ser primera
        for (int i = 0; i < totalDireccionesCuadrado - 1; i++) {
          direccionesCuadradoGuardadas[i] = direccionesCuadradoGuardadas[i + 1];
        }
        totalDireccionesCuadrado--;

        // Ejecutar el giro
        Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
        delay(delayBase);
        if (dir == 1) girar(1);
        else girar(0);

        forzarSalidaCuadradoEnSemi = true;  // Forzar la salida (En una semi (90°) con línea delante)
        ultimoEventoPista = EVENTO_ENTRADA_CUADRADO;
        return;
      } else {  // Si no hay marcas guardadas
        // No hay cuadrado, por lo tanto es el final
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
      lastError = 0;
      integral = 0;
      ultimoEventoPista = EVENTO_CRUCE_RECTO;
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
