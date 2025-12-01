const int distDetec = 166;

int dishtancia = 0;

void labirint() {
  bool line = false;
  delay(1000);
  digitalWrite(LED, HIGH);
  Motor(20, 20);
  delay(200);
  Motor(0, 0);
  while (true) {
    qtr.read(sensorValues);
    Motor(-20, -20);
    if (sensorValues[0] > 4000) {
      while (true) {
        qtr.read(sensorValues);
        Motor(0, -20);
        if (sensorValues[7] > 4000) {
          Motor(0, 0);
          break;
        }
      }
      break;
    }
    if (sensorValues[7] > 4000) {
      while (true) {
        qtr.read(sensorValues);
        Motor(-20, 0);
        if (sensorValues[0] > 4000) {
          Motor(0, 0);
          break;
        }
      }
      break;
    }
  }
  while (sensorValues[0] > 4000 && sensorValues[7] > 4000) {
    qtr.read(sensorValues);
    Motor(20, 20);
  }
  digitalWrite(LED, LOW);
  delay(200);
  digitalWrite(LED, HIGH);
  while (true) {
    while (true) {
      dishtancia = 0;
      if (lox.isRangeComplete()) {
        dishtancia = lox.readRange();
      }
      SerialBT.println(dishtancia);
      manejoUltra();
      qtr.read(sensorValues);
      if (sensorValues[0] > 3800) {
        line = true;
        while (sensorValues[7] < 3800) {
          qtr.read(sensorValues);
          Motor(0, 50);
        }
        Motor(0, 0);
        break;
      } else if (sensorValues[7] > 3800) {
        line = true;
        while (sensorValues[0] < 3800) {
          qtr.read(sensorValues);
          Motor(50, 0);
        }
        Motor(0, 0);
        break;
      }
      if (dishtancia < distDetec && dishtancia > distDetec - 25) {
        break;
      }
      delay(25);
    }
    if (line) {
      while (sensorValues[0] > 3800 && sensorValues[7] > 3800) {
        qtr.read(sensorValues);
        Motor(velocidadBaseIzq - (velocidadBaseIzq - 10), velocidadBaseDer - (velocidadBaseDer - 10));
      }
      blockLabirint = true;
      break;
    }

    SerialBT.print("Labirint | Giro 90, ");
    SerialBT.println(dishtancia);

    girarIzquierda(84);

    Motor(0, 0);
    delay(1000);

    dishtancia = 0;
    if (lox.isRangeComplete()) {
      dishtancia = lox.readRange();
    }

    if (dishtancia < distDetec && dishtancia != 0) {
      SerialBT.print("Labirint | Giro 180, ");
      SerialBT.println(dishtancia);

      girarDerecha(175);
    }
  }
}

#include "Pines.h"
#include <Ultrasonic.h>

Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

float setpoint = 10.0;  // Distancia deseada (cm)
float UKp = 2.0;        // Ajustar según tu robot
float UKi = 0.2;
float UKd = 1.0;

float Uerror, UlastError = 0;
float Uintegral = 0;
float Uderivative;
float UpidOutput;

// ---------- OTROS ----------
int UbaseSpeed = 50;  // Velocidad base de los motores (0–255)
unsigned long UlastTime = 0;
float Udt = 0.0;

void manejoUltra() {
  // ------- Lectura del ultrasonido -------
  float Udistance = ultrasonic.distanceRead();  // en cm

  // ------- Cálculo de dt -------
  unsigned long Unow = millis();
  Udt = (Unow - UlastTime) / 1000.0;
  UlastTime = Unow;

  // ------- PID -------
  Uerror = setpoint - Udistance;
  Uintegral += Uerror * Udt;
  Uderivative = (Uerror - UlastError) / Udt;
  UpidOutput = UKp * Uerror + UKi * Uintegral + UKd * Uderivative;
  UlastError = Uerror;

  // ------- Control de motores -------

  // Si el robot está muy cerca de la pared → alejar (corregir hacia el otro lado)
  // Si está lejos → acercarse
  // pidOutput modificará la diferencia de velocidad entre motores

  int leftMotorSpeed = constrain(UbaseSpeed - UpidOutput, 0, 255);
  int rightMotorSpeed = constrain(UbaseSpeed + UpidOutput, 0, 255);

  Motor(leftMotorSpeed, rightMotorSpeed);
}