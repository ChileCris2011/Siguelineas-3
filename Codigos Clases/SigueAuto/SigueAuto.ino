/********************************************************************

  Por: Ccris01 (ChileCris2011) para el equipo 'Tres absoluto (|3|)'  

  Auto V1.2              Espressif ESP32: V2.0.14 (3.x no funciona)              

*********************************************************************/

#include "Pines.h"

#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <QTRSensors.h>

// Frecuencia y resolución de los canales PWM (ledC)
const int frecuencia = 5000;
const int resolution = 8;

// Flags de guardado de dirección en los cuadrados
int primerCuadrado = -1;
int segundoCuadrado = -1;

// Flag para guardar el estado del cuadrado actual, o si no se está en uno
int inCuadrado = -1;

// Configura por donde se evadirá un objeto si es detectado
const int evadirHacia = 0;  // 0 Izquierda | 1 Derecha

// Declaración del sensor QTR
QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
int ultimaPosicion = 0;

// Variable para guardar los umbrales de cada sensor QTR
int umbral[SensorCount];

// Declaración del sensor láser (L0X)
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  // Inicializaión de la comunicación serial
  Serial.begin(115200);

  // Inicialización de motores y puente h
  inicializarMotores();

  // Configuración del sensor QTR
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){ 36, 39, 34, 35, 32, 33, 25, 26 }, SensorCount);
  qtr.setEmitterPin(27);

  // Inicialización del LED integrado
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Configuración del sensor láser (L0X)
  if (!lox.begin()) {
    Serial.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1) {
      digitalWrite(LED, HIGH);
      delay(1000);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(1000);
    }
  }

  Serial.println(F("VL53L0X iniciado correctamente"));

  // Espera al botón para la calibración
  while (digitalRead(BOTON) == 0) {}
  delay(500);

  // Empieza a girar sobre su eje
  Motor(-100, 100);

  // Calibración de los sensores QTR para seguir la línea
  for (int i = 0; i < 100; i++) {
    qtr.calibrate();
    digitalWrite(LED, HIGH);
    delay(10);
    digitalWrite(LED, LOW);
    delay(10);
  }

  // Se detiene el robot
  Motor(0, 0);

  // Configuración del umbral manual para cada sensor
  for (int i = 0; i < SensorCount; i++) {
    umbral[i] = (qtr.calibrationOn.minimum[i] + qtr.calibrationOn.maximum[i]) / 2;
  }

  delay(500);
  // Recordar poner el robot mirando hacia atrás, ya que en la caliración quedará mirando para el otro lado y empezará automáticamente
}

void loop() {

  ///////////////////////////////// Obstáculo /////////////////////////////////

  VL53L0X_RangingMeasurementData_t measure;  // Declara la variable para almacenar los datos de la medición

  // Realiza la medición de distancia y almacena los resultados en la variable 'measure'
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {
    if (measure.RangeMilliMeter < 50) {  // Detecta un objeto (~5cm)
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
      while (sensorValues[3] < umbral[3] || sensorValues[4] < umbral[4]) {
        qtr.read(sensorValues);  // Avanza hasta detectar la línea
        Motor(50, 50);
      }
      Motor(0, 0);
      delay(200);
      Motor(50, 50);  // Avanza un poco para girar bien
      delay(250);
      qtr.read(sensorValues);
      while (sensorValues[4] < umbral[4]) {
        qtr.read(sensorValues);
        girar(evadirHacia);  // Gira hasta acomodarse en la línea
      }
    }
  }

  ///////////////////////////////// Giros 90 | Intersecciones /////////////////////////////////

  qtr.read(sensorValues);
  if (sensorValues[0] < umbral[0] && sensorValues[1] < umbral[1] && sensorValues[2] < umbral[2] && sensorValues[3] < umbral[3] && sensorValues[4] < umbral[4] && sensorValues[5] > umbral[5] && sensorValues[6] > umbral[6] && sensorValues[7] > umbral[7]) {
    interseccion();  // Intersección
  } else if (sensorValues[0] > umbral[0] && sensorValues[1] > umbral[1] && sensorValues[2] > umbral[2] && sensorValues[3] > umbral[3] && sensorValues[4] > umbral[4]) {
    giros(0);  // Izquierda
  } else if (sensorValues[7] > umbral[7] && sensorValues[6] > umbral[6] && sensorValues[5] > umbral[5] && sensorValues[4] > umbral[4] && sensorValues[3] > umbral[3]) {
    giros(1);  // Derecha
  }

  ///////////////////////////////// Corrección /////////////////////////////////

  // Verificar si todos los sensores están "apagados" (sin línea)
  bool sinLinea = true;
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] > umbral[i]) {
      sinLinea = false;
      break;
    }
  }

  int posicion;
  if (sinLinea) {  // No hay línea
    if (ultimaPosicion > 240) {
      posicion = 255;
    } else if (ultimaPosicion < -240) {  // Trata de corregir la posición según la última lectura
      posicion = -255;
    } else {
      posicion = 0;  // línea perdida sin dirección clara
    }
  }

  ///////////////////////////////// Sigue Líneas /////////////////////////////////

  else { // Sí tenemos línea
    posicion = qtr.readLineBlack(sensorValues);
    posicion = map(posicion, 0, 7000, -1000, 1000); // Se mapea a un valor entre -255 y 255 (No se que pasa aqui)
    posicion = constrain(posicion, -255, 255);
    ultimaPosicion = posicion;
  }

  if (posicion >= -50 && posicion <= 50) {
    Motor(50, 50);
  } else if (posicion < -50 && posicion > -150) {
    Motor(0, 100);
  } else if (posicion > 50 && posicion < 150) {
    Motor(100, 0);
  } else if (posicion > 150) {  // Sigue la línea
    Motor(150, 0);
  } else if (posicion < -150) {
    Motor(0, 150);
  }
}