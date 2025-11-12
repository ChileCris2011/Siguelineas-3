void fEvasion() {
  if (!(contador == evasion)) {
    return;
  }
  
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