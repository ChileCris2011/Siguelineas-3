void evaluarCruce() {

  static int contadorCruce = 0;

  /*
  Ahora se llama directamente a la función evaluarCruce() y se hace la verificación
  dentro para evitar que el PID actúe en los giros, enchuecando el robot
  */

  while (contadorCruce < 3) {
    qtr.read(sensorValues);
    if (sensorValues[0] > 4000 || sensorValues[7] > 4000) contadorCruce++;
    else loop();
  }

  // Umbrales
  const int TH_LADO = 4000;    // extremos (0 y 7)
  const int TH_CENTRO = 3800;  // centrales (2..5) para "hay línea al frente"

  Motor(velocidadBaseIzq - minusGiro, velocidadBaseDer - minusGiro);

  // (3) Avanzar ESCANEANDO para clasificar
  bool vioIzq = false, vioDer = false;
  unsigned long t0 = millis();
  while (millis() - t0 < 100) {
    qtr.read(sensorValues);
    if (sensorValues[0] > TH_LADO) vioIzq = true;
    if (sensorValues[7] > TH_LADO) vioDer = true;
    for (int i = 2; i <= 4; i++) {
      if (sensorValues[i] > TH_CENTRO) {
        break;
      }
    }

    if (vioIzq && vioDer) {
      break;
    }
  }

  qtr.read(sensorValues);

  digitalWrite(LED, HIGH);
  while (sensorValues[0] > TH_LADO || sensorValues[7] > TH_LADO) {
    qtr.read(sensorValues);
    Motor(velocidadBaseIzq - minusGiro, velocidadBaseDer - minusGiro);
  }
  digitalWrite(LED, LOW);

  // (4) Detenerse
  Motor(0, 0);
  delay(25);

  // (5) Revisar si hay línea al frente (lectura estática final)
  qtr.read(sensorValues);
  bool hayLineaFinal = false;
  for (int i = 2; i <= 4; i++) {
    if (sensorValues[i] > TH_CENTRO) {
      hayLineaFinal = true;
      break;
    }
  }

  // (6) Tomar decisión

  // --- SEMI-INTERSECCIÓN (solo un lado) ---
  if (vioIzq ^ vioDer) {        // Si SOLO vio UN lado
    if (hayLineaFinal) {        // Si hay línea delante
      if (forzarProximaSemi) {  // Si tiene que forzar la salida
        Motor(velocidadBaseIzq - minusGiro, velocidadBaseDer - minusGiro);
        delay(delayGiro);
        if (vioIzq) {
          girarCrudo(0);
          delay(666);
          giroWhile(0);
        } else {
          girarCrudo(1);
          delay(666);
          giroWhile(1);
        }
        forzarProximaSemi = false;  // consumir la orden
        // Marcar posicion giroscopio ** <- ???
        //puedeLaser = true;  // Activa la detección (Cambiar segun la ubicacion del obstaculo)
        return;
      }

      // Si NO hay forzado: guardar marca
      if (totalMarcasGuardadas < 2) {  // Si hay menos de 2 marcas guardadas
        if (vioIzq) {
          marcaCuadradoDir[totalMarcasGuardadas] = -1;
        }  // Guarda la marca en el lado que vió
        if (vioDer) {
          marcaCuadradoDir[totalMarcasGuardadas] = +1;
        }
        totalMarcasGuardadas++;
      }

      Motor(velocidadBaseIzq - minusGiro, velocidadBaseDer - minusGiro);
      delay(140);
      Motor(0, 0);
      return;  // volver al PID
    } else {
      // SEMI sin línea → giro normal inmediato
      Motor(velocidadBaseIzq - minusGiro, velocidadBaseIzq - minusGiro);
      delay(delayGiro);
      if (vioIzq) {
        giroWhile(0);
        puedeLaser = true;
        return;
      }
      if (vioDer) {
        giroWhile(1);
        puedeLaser = true;
        return;
      }
    }
  }

  // --- INTERSECCIÓN COMPLETA (ambos lados) ---
  if (vioIzq && vioDer) {
    if (!hayLineaFinal) {              // Si no hay linea delante
      if (totalMarcasGuardadas > 0) {  // Si hay marcas guardadas (Hay cuadrado)
        int dir = marcaCuadradoDir[0];

        // Desplazar las marcas para que la segunda pase a ser primera
        for (int i = 0; i < totalMarcasGuardadas - 1; i++) {
          marcaCuadradoDir[i] = marcaCuadradoDir[i + 1];
        }
        totalMarcasGuardadas--;

        // Ejecutar el giro
        Motor(velocidadBaseIzq - minusGiro, velocidadBaseDer - minusGiro);
        delay(delayGiro);
        if (dir < 0) giroWhile(0);
        else giroWhile(1);

        forzarProximaSemi = true;  // Forzar la salida (En una semi (90°) con línea delante)
        return;
      } else {  // Si no hay marcas guardadas
        //No hay cuadrado, por lo tanto es el final
        Motor(0, 0);
        delay(500);  // Se detiene
        for (int i = 0; i < 3; i++) {
          digitalWrite(LED, HIGH);  // Juego de luces (3)
          delay(500);
          digitalWrite(LED, LOW);
          delay(500);
        }
        while (true) {}  // fin
      }
    } else {  // Intersección con línea delante
      // Se puede saltar con normalidad (espero...)
      Motor(velocidadBaseIzq - minusGiro, velocidadBaseDer - minusGiro);
      delay(140);
      Motor(0, 0);
      return;
    }
  }

  // --- Nada concluyente ---

  /*
  Si el código ha llegado hasta aquí, es que no ha ocurrido nada de lo especificado arriba.
  Aunque es raro...
  ¡No te preocupes, probablemente no sea el robot!
  Sino el programador...
  */

  Motor(0, 0);
  while (true) {
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
  }
}