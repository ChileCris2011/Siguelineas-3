bool canUtra = true;
unsigned long lastRec = 0;

void laberinto() {
  digitalWrite(LED, HIGH);
  Motor(0, 0);
  delay(1000);
  while (true) {
    delay(100);
    lab = true;
    if (canUtra) {
      manejoUltra();
    } else {
      Motor(50, 50);
    }
    qtr.read(sensorValues);
    SerialBT.print("L0X: ");
    distancia = 0;
    if (lox.isRangeComplete()) {
      distancia = lox.readRange();
    }
    SerialBT.println(distancia);
    if (distancia < 370 && distancia != 0) {
      SerialBT.print("Labirint | Wall... (");
      SerialBT.print(distancia);
      SerialBT.print(") ");
      Motor(0, 0);
      Motor(75, 70);
      delay(1900);
      Motor(0, 0);
      SerialBT.println("90°");
      girarDerecha(88);
      delay(200);
      distancia = 0;
      if (lox.isRangeComplete()) {
        distancia = lox.readRange();
      }
      delay(200);
      distancia = 0;
      if (lox.isRangeComplete()) {
        distancia = lox.readRange();
      }
      if (distancia < 350 && distancia != 0) {
        SerialBT.print("Labirint | Plex... (");
        SerialBT.print(distancia);
        SerialBT.print(") ");
        Motor(0, 0);
        unsigned long starst = millis();
        while (millis() - starst < 2345) {
          distancia = 0;
          if (lox.isRangeComplete()) {
            distancia = lox.readRange();
          }
          SerialBT.println(distancia);
          delay(100);
        }
        girarIzquierda(178);
        delay(200);
        distancia = 0;
        if (lox.isRangeComplete()) {
          distancia = lox.readRange();
        }
        delay(200);
        distancia = 0;
        if (lox.isRangeComplete()) {
          distancia = lox.readRange();
        }
        if (distancia < 350 && distancia != 0) {
          SerialBT.print("Labirint | No exit... (");
          unsigned long starst = millis();
          while (millis() - starst < 2345) {
            distancia = 0;
            if (lox.isRangeComplete()) {
              distancia = lox.readRange();
            }
            SerialBT.println(distancia);
            delay(100);
          }
          SerialBT.print(distancia);
          SerialBT.print(") ");
          Motor(0, 0);
          girarIzquierda(88);
          delay(200);
          distancia = 0;
          if (lox.isRangeComplete()) {
            distancia = lox.readRange();
          }
        }
      }
      lastRec = millis();
    }
    qtr.read(sensorValues);
    if ((sensorValues[0] >= umbral) || (sensorValues[7] >= umbral)) {
      SerialBT.print("Labirint | Line: Exiting...");
      Motor(0, 0);
      delay(500);
      break;
    }
    canUtra = false;
    if (millis() - lastRec > 2000){
      canUtra = true;
    }
  }
}

#include "Pines.h"
#include <Ultrasonic.h>

Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

float setpoint = 10.0;  // Distancia deseada (cm)

// ---------- OTROS ----------
int UbaseSpeedIzq = 75;
int UbaseSpeedDer = 70;

unsigned long UlastTime = 0;
float Udt = 0.0;

void manejoUltra() {
  // ------- Lectura del ultrasonido -------
  int Udistance = ultrasonic.distanceRead();  // en cm
  SerialBT.print("Labirint | Ultra: ");
  SerialBT.print(Udistance);

  SerialBT.print('\t');

  switch (Udistance) {
    case 5:
      Motor(50, 70);
      break;
    case 7:
      Motor(50, 60);
      break;
    case 10:
      Motor(50, 50);
      break;
    case 13:
      Motor(60, 50);
      break;
    case 15:
      Motor(70, 50);
      break;
    default:
      if (Udistance < 5) {
        Motor(50, 80);
      } else {
        Motor(80, 50);
      }
      break;
  }
}