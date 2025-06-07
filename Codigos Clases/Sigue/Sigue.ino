#include <QTRSensors.h>
#include <BluetoothSerial.h>

const int freq = 5000;
const int resolution = 8;
int umbral = 3500;
int contador = 0;
#define BOTON 12




QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
#define LED 2

int ultimaPosicion = 0;


void setup() {
  inicializarMotores();

  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]) {
    36, 39, 34, 35, 32, 33, 25, 26
  },
  SensorCount);
  qtr.setEmitterPin(27);
  pinMode(LED, OUTPUT);

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

  if (sensorValues[1] > umbral && sensorValues[2] > umbral && sensorValues[3] > umbral && sensorValues[0] > umbral && sensorValues[4] > umbral) {  //IZQUIERDA

    if (contador == (6 || 10 || 11 || 15 || 17 || 18 || 19)) {

      Motor(50, 50);
      delay(500);
      Motor(0, 0);

    } else {
      Motor(0, 0);
      delay(200);
      Motor(50, 50);
      delay(500);
      Motor(80, -80);  //giro izq
      delay(700);
      Motor(50, 50);
      delay(100);

      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
    }
    contador += 1;
  }


  ///////////////////////////////////////////////////////////


  else if (sensorValues[7] > umbral && sensorValues[6] > umbral && sensorValues[5] > umbral && sensorValues[4] > umbral && sensorValues[3] > umbral) {  //derecha
    if (contador == (6 || 10 || 11 || 15 || 17 || 18 || 19)) {

      Motor(50, 50);
      delay(500);
      Motor(0, 0);

    }
    else {
      Motor(0, 0);
      delay(200);
      Motor(50, 50);
      delay(500);
      Motor(-80, 80);  //giro der
      delay(700);
      Motor(50, 50);
      delay(100);

      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
    }
    contador += 1;
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
    if (sensorValues[i] > 3000) {
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
