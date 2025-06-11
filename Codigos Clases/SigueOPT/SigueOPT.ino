#include "Pines.h"

#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <QTRSensors.h>
#include <BluetoothSerial.h>

///////// Configuración /////////////////////////////////////////////////////////////////////////////////////////////////////////

// PWM
//Por defecto, estas configuraciones deberían estar bien
const int frecuencia = 5000;
const int resolution = 8;
const int umbral = 3500;

// Hitos
//Los hitos se cuentan desde el hito 0. Los hitos son giros o intersecciones que el robot debe superar.
//Para los hitos, no se cuentan los giros de los cuadrados para evitar inconsistencias. Solo se cuenta el hito de entrada,
//no el se salida.
const int intersecciones[] = { 0, 10, 14 };  // Intersecciones (Pasar de largo / Ignorar)
const int lIntersecciones = 3;               // Longitud de intersecciones (para el for)

const int evasión = 8;  //Hito de evasión (Hito antes a la evasión del obstáculo + 1)

const int final = 24;  //Hito final (Hito de la meta)

const int primeraMarca = 0;  //Hito donde se avisa la dirección de un cuadrado. Dejar en -1 si debe ser ignorada
const int segundaMarca = 0;  //Hito donde se avisa la dirección de un segundo cuadrado. Dejar en -1 si no hay o debe ser ignorada

const int primerCuadrado = 0;   //Hito donde empieza el primer cuadrado. Dejar en -1 si no hay
const int segundoCuadrado = 0;  //Hito donde empieza el segundo cuadrado. Dejar en -1 si no hay

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

bool hInt = false;  //Variable para verificar si se está en un hito de una intersección

bool inCuadrado = false;  //Variable para verificar si se está en un cuadrado

int primerGiro = -1;  //0 izquierda, 1 derecha

int segundoGiro = -1;  //0 izquierda, 1 derecha

int contador = 0; //Contador de hitos

QTRSensors qtr;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
int ultimaPosicion = 0;

void setup() {
  inicializarMotores();

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){
                      36, 39, 34, 35, 32, 33, 25, 26 },
                    SensorCount);
  qtr.setEmitterPin(27);
  pinMode(LED, OUTPUT);

  if (!lox.begin()) {
    Serial.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1)
      ;  // Detiene el programa si no se puede inicializar el sensor
  }

  Serial.println(F("VL53L0X iniciado correctamente"));

  while (digitalRead(BOTON) == 0) {
  }
  delay(500);

  for (int i = 0; i < 100; i++) {
    qtr.calibrate();
    digitalWrite(LED, HIGH);
    delay(10);
    digitalWrite(LED, LOW);
    delay(10);
  }

  Serial.begin(115200);


  while (digitalRead(BOTON) == 0) {
  }
  delay(500);
}

void loop() {
  // Leer sensores
  qtr.read(sensorValues);

  if (contador == evasion) {
    VL53L0X_RangingMeasurementData_t measure;  //Declara la variable para almacenar los datos de la medición

    //Realiza la medición de distancia y almacena los resultados en la variable 'measure'
    lox.rangingTest(&measure, false);

    if (measure.RangeStatus != 4) {
      if (measure.RangeMilliMeter < 100) {
        Motor(-50, -50);
        delay(500);
        Motor(0, 0);
        delay(200);
        Motor(80, -80);
        delay(700);
        Motor(50, 50);
        delay(1500);
        Motor(0, 0);
        delay(200);
        Motor(-80, 80);
        delay(700);
        Motor(50, 50);
        delay(3250);
        Motor(0, 0);
        delay(200);
        Motor(-80, 80);
        delay(700);
        while (sensorValues[3] < umbral || sensorValues[4] < umbral) {
          qtr.read(sensorValues);
          Motor(50, 50);
        }
        Motor(0, 0);
        delay(200);
        Motor(50, 50);
        delay(250);
        qtr.read(sensorValues);
        while (sensorValues[4] < umbral) {
          qtr.read(sensorValues);
          Motor(80, -80);
        }
        Motor(50, 50);
        delay(200);
        Motor(0, 0);
      }
    }
  }

  for (int i = 0; i < lIntersecciones; i++) {
    if (contador == intersecciones[i]) {
      hInt = true;
      break;
    } else {
      hInt = false;
    }
  }

  if (sensorValues[1] > umbral && sensorValues[2] > umbral && sensorValues[3] > umbral && sensorValues[0] > umbral && sensorValues[4] > umbral) {  //IZQUIERDA
    if (!inCuadrado) {
      if (hInt) {

        pass();

      } else if (contador == primeraMarca) {
        primerGiro = 0;
        pass();
      } else if (contador == segundaMarca) {
        segudoGiro = 0;
        pass();
      } else if (contador == primerCuadrado) {
        if (primerGiro) {
          giroDer();
        } else {
          giroIzq();
        }
        inCuadrado = true;
      } else if (contador == segundoCuadrado) {
        if (segundoGiro) {
          giroDer();
        } else {
          giroIzq();
        }
        inCuadrado = true;
      } else if (contador == final) {
        Motor(0, 0);
        while (true) {
          delay(200);
          digitalWrite(LED, HIGH);
          delay(200);
          digitalWrite(LED, LOW);
        }
      } else {
        giroIzq();
      }
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      contador += 1;
    } else {
      giroIzq();
      if (primerGiro == 0 || segundoGiro == 0) {  // Si el cuadrado se tuvo que tomar por la izquierda, entonces todos los giros
        inCuadrado = false;                       // dentro del cuadrado serán por la derecha. Por tanto, si se gira a la izquierda,
      }                                           // se ha salido del cuadrado.
    }
  }


  ///////////////////////////////////////////////////////////


  else if (sensorValues[7] > umbral && sensorValues[6] > umbral && sensorValues[5] > umbral && sensorValues[4] > umbral && sensorValues[3] > umbral) {  //derecha
    if (!inCuadrado) {
      if (hInt) {

        pass();

      } else if (contador == primeraMarca) {
        primerGiro = 1;
        pass();
      } else if (contador == segundaMarca) {
        segudoGiro = 1;
        pass();
      } else if (contador == primerCuadrado) {
        if (primerGiro) {
          giroDer();
        } else {
          giroIzq();
        }
        inCuadrado = true;
      } else if (contador == segundoCuadrado) {
        if (segundoGiro) {
          giroDer();
        } else {
          giroIzq();
        }
        inCuadrado = true;
      } else if (contador == final) {
        Motor(0, 0);
        while (true) {
          delay(200);
          digitalWrite(LED, HIGH);
          delay(200);
          digitalWrite(LED, LOW);
        }
      } else {
        giroDer();
      }
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      contador += 1;
    } else {
      giroDer();
      if (primerGiro == 0 || segundoGiro == 0) {  // Si el cuadrado se tuvo que tomar por la derecha, entonces todos los giros
        inCuadrado = false;                       // dentro del cuadrado serán por la izquierda. Por tanto, si se gira a la derecha,
      }                                           // se ha salido del cuadrado.
    }
  }


  // Mostrar valores por Serial
  for (uint8_t i = 0; i < SensorCount; i++) {
    int valor = sensorValues[i];
    if (valor == 4095) {
      valor = valor * 1.8;
    }
    Serial.print(valor);
    Serial.print('\t');
  }

  // Verificar si todos los sensores están "apagados" (sin línea)
  bool sinLinea = true;
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] > umbral) {
      sinLinea = false;
      break;
    }
  }

  int posicion;
  if (sinLinea) {
    if (ultimaPosicion > 240) {
      posicion = 255;
    } else if (ultimaPosicion < -240) {
      posicion = -255;
    } else {
      posicion = 0;  // línea perdida sin dirección clara
    }
  } else {
    posicion = qtr.readLineBlack(sensorValues);
    posicion = map(posicion, 0, 7000, -1000, 1000);
    posicion = constrain(posicion, -255, 255);
    ultimaPosicion = posicion;
  }
  /////////////////////////////////

  ////////////////////////////////
  if (posicion >= -50 && posicion <= 50) {
    Motor(50, 50);
  } else if (posicion < -50 && posicion > -150) {
    Motor(0, 80);
  } else if (posicion > 50 && posicion < 150) {
    Motor(80, 0);
  } else if (posicion > 150) {  //seguidor
    Motor(120, 0);
  } else if (posicion < -150) {
    Motor(0, 120);
  }
  ////////////////////////////////////

  Serial.print(posicion);
  Serial.println('\t');
}
