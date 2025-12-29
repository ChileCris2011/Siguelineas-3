void inicializarQTR(bool calibrar) {
  qtr.setTypeAnalog();                 // QTR en modo análogo (lecturas/pins analogicos)
  qtr.setSensorPins((const uint8_t[]){ // Configurar pines QTR
                                       36, 39, 34, 35, 32, 33, 25, 26 },
                    SensorCount);
  qtr.setEmitterPin(27);        // Emitter pin
  if (calibrar) calibrarQTR();  // Calibrar
}

void calibrarQTR(uint16_t t) {
  for (uint16_t i = 0; i < t; i++) qtr.calibrate();
}

void evaluarCruce() {
  noPID = true;

  static int contadorCruce = 0;

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

  // (2) Retroceder
  Motor(-velBaseIzq * 0.75, -velBaseDer * 0.75);
  delay(tRetroceso != -1 ? tRetroceso : 0);
  Motor(0, 0);

  // (3) Avanzar escaneando para clasificar
  Motor(velBaseIzq * 0.75, velBaseDer * 0.75);

  bool vioIzq = false, vioDer = false;
  unsigned long t0 = millis();

  while (millis() - t0 < tScan) {
    qtr.read(sensorValues);

    if (sensorValues[0] > umbral) vioIzq = true;
    if (sensorValues[7] > umbral) vioDer = true;

    if (vioIzq && vioDer) {
      break;  // Si ya vió ambos lados, ¿que más vamos a escanear?
    }
  }

  int ladoGiro = 0;  // Digo, si vamos a estar comprobando cada vez dentro de la roma de decisión, es mejor tener ya la direccion aquí.
  if (vioIzq && !vioDer) {
    ladoGiro = Izquierda;
  } else if (!vioIzq && vioDer) {
    ladoGiro = Derecha;
  }

  SerialBT.printf("vioIzq = %i  vioDer = %d  ladoGiro = %l", vioIzq, vioDer, ladoGiro);

  // (3) Acomodarse después de la línea
  while (sensorValues[0] > umbral || sensorValues[7] > umbral) {
    qtr.read(sensorValues);
    Motor(velBaseIzq * 0.75, velBaseDer * 0.75);
  }

  // (4) Detenerse
  Motor(0, 0);

  // (5) Revisar si hay línea al frente
  bool hayLineaFinal = false;

  qtr.read(sensorValues);
  if (sensorValues[3] > umbral || sensorValues[4] > umbral) {
    hayLineaFinal = true;
  }

  SerialBT.printf("  Hay Línea = %l", hayLineaFinal ? "si" : "no");

  // (6) Revisar la distancia delante
  int distLab = 0;

  if (lox.isRangeComplete()) {
    distLab = lox.readRange();
  }

  SerialBT.printf("distLab = %dmm\n", distLab);

  // (7) Tomar decisión
  //// Semi (Solo un lado)
  if (vioIzq ^ vioDer) {  // ^ = XOR, si solo uno es verdadero
    // Forzado
    if (hayLineaFinal) {        // Si hay línea delante
      if (forzarProximaSemi) {  // SI tiene que forzar el giro para salir del cuadrado
        SerialBT.println("Saliendo del cuadrado...");

        Motor(velBaseIzq * 0.75, velBaseDer * 0.75);
        delay(delayBase);

        giroWhile(ladoGiro, true);

        forzarProximaSemi = false;  // Consumir la orden

        return;
      }

      // Marca
      if (totalMarcasGuardadas < 2) {  // Si hay menos de 2 marcas guardadas. Ésto se podría cambiar para, en vez de rechazar la siguiente marca, correr la lista.
        SerialBT.println("Guardando marca...");

        marcaCuadradoDir[totalMarcasGuardadas] = ladoGiro;

        totalMarcasGuardadas++;
      }
      // Aquí técnicamente ya estamos en blanco, así que no es necesario avanzar. Si no... agreguen un avanzar y ya...
      return;
    } else {  // Entonces si no hay línea
      // 90s
      SerialBT.println("Giro 90...");
      Motor(velBaseIzq * 0.75, velBaseDer * 0.75);
      delay(delayBase);
      giroWhile(ladoGiro);
      return;
    }
  }

  //// Intersección (ambos lados)
  if (vioIzq && vioDer) {
    // Fin de carrera
    if (!hayLineaFinal) {  // No hay línea delante
      // Laberinto (si, claro)


      // Cuadrado
      if (totalMarcasGuardadas > 0) {  // Si hay marcas guardadas
        SerialBT.println("Cuadrado...");
        int dir = marcaCuadradoDir[0];

        // Desplazar la lista
        for (int i = 0; i < totalMarcasGuardadas - 1; i++) {
          marcaCuadradoDir[i] = marcaCuadradoDir[i + 1];
        }
        totalMarcasGuardadas--;

        // Ejecutar el giro
        Motor(velBaseIzq * 0.75, velBaseDer * 0.75);
        delay(delayBase);
        giroWhile(dir);

        forzarProximaSemi = true;  // Forzar la salida en la próxima semi

        return;
      } else {  // Si no hay marcas guardadas
        // Final
        SerialBT.println("¡Final!");
        Motor(0, 0);
        delay(500);
        blink(3, 500);   // 3 luces
        while (true) {}  // fin
      }
    } else {  // Hay linea delante
      // Cruce
      // Se puede saltar con normalidad... espero...
      SerialBT.println("Cruce...");
      // Nuevamente... Técnicamente ya salimos de la línea así que no debería haber problema. Si no ponle un avanzar y ya...
      return;
    }
  }
  //// Nada concluyente
  /*
    Si el código ha llegado hasta aquí, quiere decir que ninguna de las condiciones de arriba se han complido.
    Ésto es algo raro... Aunque ha pasado algunas veces, en rampas por ejemplo.
    Puedes echarle la culpa a un millón de cosas, como la luz, el ambiente, el que hizo la pista,
    el que tenía el robot, el robot, los sensores, un raayo cósmico, el ben, etc...
    pero NUNCA al programador ;)
  */

  SerialBT.println("Nada concluyente...");

  Motor(velBaseIzq * 0.25, velBaseDer * 0.25);
}