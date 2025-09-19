void linea() {
  if (sensorValues[0] < umbral && sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] > umbral && sensorValues[4] > umbral && sensorValues[5] < umbral && sensorValues[6] < umbral && sensorValues[7] < umbral) {
    Motor(80, 80);
    //rec
  }
  else if (sensorValues[0] > umbral && sensorValues[1] > umbral && sensorValues[2] > umbral && sensorValues[3] > umbral && sensorValues[4] > umbral && sensorValues[5] > umbral && sensorValues[6] > umbral && sensorValues[7] > umbral) {
    intersecciones++;
    Motor(80, 80);
    delay(250);
  }
  else if (sensorValues[0] < umbral && sensorValues[1] < umbral && sensorValues[2] > umbral && sensorValues[3] > umbral && sensorValues[4] < umbral && sensorValues[5] < umbral && sensorValues[6] < umbral && sensorValues[7] < umbral) {
    Motor(20, 50);
    //izd
  }
  else if (sensorValues[0] < umbral && sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] < umbral && sensorValues[4] > umbral && sensorValues[5] > umbral && sensorValues[6] < umbral && sensorValues[7] < umbral) {
    Motor(50, 20);
    //ded
  }
  else if (sensorValues[0] > umbral && sensorValues[1] > umbral && sensorValues[2] > umbral && sensorValues[3]  > umbral && sensorValues[4] > umbral && sensorValues[5] < umbral && sensorValues[6] < umbral && sensorValues[7] < umbral) {
    Motor(80, 80);
    delay(90);
    Motor(-50, 10);
    delay(550);
    //izF
  }
  else if (sensorValues[0] < umbral && sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] > umbral && sensorValues[4] > umbral && sensorValues[5] > umbral && sensorValues[6] > umbral && sensorValues[7] > umbral) {
    Motor(80, 80);
    delay(90);
    Motor(10, -50);
    delay(550);
    //deF
  }
  else if (sensorValues[0] > umbral && sensorValues[1] > umbral && sensorValues[2] < umbral && sensorValues[3]  < umbral && sensorValues[4] < umbral && sensorValues[5] < umbral && sensorValues[6] < umbral && sensorValues[7] < umbral) {
    Motor(20, 70);
    //izm
  }
  else if (sensorValues[0] < umbral && sensorValues[1] < umbral && sensorValues[2] < umbral && sensorValues[3] < umbral && sensorValues[4] < umbral && sensorValues[5] > umbral && sensorValues[6] > umbral && sensorValues[7] < umbral) {
    Motor(70, 20);
    //dem
  }


  if (sensorValues[0] > umbral) {
    Motor(200, -200);
  }
  else if (sensorValues[7] > umbral) {
    Motor(-200, 200);
  }
}
