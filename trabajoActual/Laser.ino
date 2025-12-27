void inicializarLaser() {
  if (!lox.begin()) {
    Serial.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1) {
      blink(1, 1000, 500);
    }
  }
  lox.startRangeContinuous();  // Empieza la lectura contínua del láser
}

void evadir() {  // Por si no lo sabías: Evadir.
  SerialBT.printf("Obstáculo detectado (%d)\n", lecturaMM);
  Motor(-50, -50);  // Retrocede para no golpear el objeto al girar
  delay(700);
  Motor(0, 0);
  delay(100);
  Girar(90, evadirHacia);  // Gira por el lado indicado
  Motor(50, 50);
  delay(1500);
  Motor(0, 0);
  delay(100);
  Girar(90, evadirHacia * -1);  // Gira hacia el lado contrario (Mas facil de entender, ¿no?)
  Motor(50, 50);
  delay(4250);  // ¿Por que seguimos con el delay? Digo, es confiable, pero si queremos aumentar la velocidad, es algo lento. Por cierto, tecnicamente si podemos leer la velocidad en la que avanza el robot con el giroscopio, podemos calcular **cuanto** avanza el robot...
  Motor(0, 0);
  delay(100);
  Girar(90, evadirHacia * -1);  // Girar al lado contrario
  Motor(50, 50);
  delay(750);  // Técnicamente la distancia de retorno es mayor o igual a la del inicio, así que avanzamos "ciegamente" la mitad por si hay alguna linea entre medio.
  qtr.read(sensorValues);
  while (sensorValues[3] < umbral || sensorValues[4] < umbral) {
    qtr.read(sensorValues);
    Motor(50, 50);
  }
  Motor(0, 0);
  delay(100);
  Motor(50, 50);
  delay(666);  // Avanza un poco para girar bien
  giroWhile(evadirHacia);
  contlaser++;
  // puedeLaser = false; // Evitar falsos positivos

  if (contlaser >= autolock && autolock != -1) {
    blockLaser = true;
  }
}