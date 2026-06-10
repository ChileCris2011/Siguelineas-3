#include "Pines.h"
//Librería para el sensor IR
#include <QTRSensors.h>

const int freq = 5000;
const int resolution = 8;

//Se crea el objeto del qtr
QTRSensors qtr;

//Se crea el arreglo para almacenar las lectura
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

//Se definen umbrales para cada sensor (valores arbitrarios, en la practica se debe ajustar)
int umbrales[SensorCount] = {4000,4000,4000,4000,4000,4000,4000,4000};

void setup() {
  inicializarMotores();
  inicializarSensores();
  //La funcion calibrar sensores espera a que se apriete el botón para iniciar
  //Mientras se hace el proceso de calibración se encenderá el LED de la esp32
  //Cuando finalice la calibración, se espera a que se vuelva apretar el botón para iniciar el programa
  calibrarSensores();
}

void loop() {
  //Se actualiza la lectura del sensor
  qtr.read(sensorValues);
  int posicion = qtr.readLineBlack(sensorValues);
  posicion = map(posicion, 0, 7000, -255, 255);  
  //Se hace un siguelíneas simple de 3 casos
  //El robot está relativamente centrado
  if (posicion <= 50 and posicion >= -50){
    Motor(100,100);
  //La línea se encuentra a la izquierda del robot
  }else if (posicion < -50){
    Motor(0,100);
  //La línea se encuentra a la derecha del robot
  }else if(posicion > 50){
    Motor(100,0);
  } 
}
