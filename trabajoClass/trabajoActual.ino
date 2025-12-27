#include "header.h"

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Project Classifier");
  Motores.begin();
  pinMode(LED, OUTPUT);
  pinMode(BOTON, INPUT);

  esperarBoton();

  Line.begin(sensorValues);
  Laser.begin();

  parpadeo(1, 200);

  Gyro.begin();
}

void loop() {

  // laser();

  qtr.read(sensorValues);  // Lectura de los sensores de línea

  Line.filter();

  // Posición manual ponderada
  uint32_t sumaPesada = 0;
  uint32_t sumaTotal = 0;
  for (uint8_t i = 0; i < SensorCount; i++) {
    sumaPesada += (uint32_t)sensorValues[i] * (i * 1000);
    sumaTotal += sensorValues[i];
  }
  uint16_t position = (sumaTotal > 0) ? (sumaPesada / sumaTotal) : 0;

  // ---- NUEVO: manejo de GAPS (todo blanco) ----
  if (sumaTotal == 0) {
    // Avanza recto con velocidad base para no “morir” en líneas segmentadas
    Motor(velocidadBaseIzq - 10, velocidadBaseDer - 10);
    // Evita acumular integral/derivativos inútiles
    integral = 0;
    lastError = 0;
    // No corremos PID en esta iteración
    return;
  }

  // Disparador de cruce por extremos
  if (sensorValues[0] > 4000 || sensorValues[7] > 4000) evaluarCruce();

  // Seguimiento de línea normal
  PID(position);
}

void evaluarCruce() {

  static int contadorCruce = 0;

  /*
    Ahora se llama directamente a la función evaluarCruce() y se hace la verificación
    dentro para evitar que el PID actúe en los giros, enchuecando el robot
  */

  // (1) Verificar para evitar falsos positivos

  while (contadorCruce < 3) {
    qtr.read(sensorValues);
    if (sensorValues[0] > 4000 || sensorValues[7] > 4000) contadorCruce++;
    else loop();
  }

  // Umbrales
  const int TH_LADO = 4000;    // extremos (0 y 7)
  const int TH_CENTRO = 4000;  // centrales (2..5) para "hay línea al frente"

  Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);

  // (2) Avanzar ESCANEANDO para clasificar
  bool vioIzq = false, vioDer = false;
  unsigned long t0 = millis();

  while (millis() - t0 < deteccionBase) {
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

  SerialBT.print("vioIzq = ");
  SerialBT.print(vioIzq);
  SerialBT.print("\t vioDer = ");
  SerialBT.print(vioDer);

  qtr.read(sensorValues);

  while (sensorValues[0] > TH_LADO && sensorValues[7] > TH_LADO) {
    qtr.read(sensorValues);
    Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
  }

  delay(55);

  // (3) Detenerse
  Motor(0, 0);

  // (4) Revisar si hay línea al frente (primera lectura estática)
  qtr.read(sensorValues);
  bool hayLineaFinal = false;
  for (int i = 2; i <= 4; i++) {
    if (sensorValues[i] > TH_CENTRO) {
      hayLineaFinal = true;
      break;
    }
  }

  SerialBT.print("\t Hay Linea = ");
  SerialBT.print(hayLineaFinal);

  // (5) Revisar la ditancia delante (lectura estática final)

  int distLab = 0;

  if (lox.isRangeComplete()) {
    distLab = lox.readRange();
  }

  SerialBT.print("\t distLab = ");
  SerialBT.println(distLab);

  // (6) Tomar decisión

  // --- SEMI-INTERSECCIÓN (solo un lado) ---
  if (vioIzq ^ vioDer) {  // Si SOLO vio UN lado
    if (hayLineaFinal) {  // Si hay línea delante

      if (forzarProximaSemi) {  // Si tiene que forzar la salida
        SerialBT.println("Forzando Semi...");
        Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
        delay(delayBase);
        if (vioIzq) {
          girarCrudo(0);
          delay(700);
          giroWhile(0);
        } else {
          girarCrudo(1);
          delay(700);
          giroWhile(1);
        }
        forzarProximaSemi = false;  // consumir la orden
        inicioCuadrado = -1;
        // Marcar posicion giroscopio ** <- ???
        //puedeLaser = true;  // Activa la detecciónb (Cambiar segun la ubicacion del obstaculo)
        return;
      }

      // Si NO hay forzado: guardar marca
      if (totalMarcasGuardadas < 2) {  // Si hay menos de 2 marcas guardadas
        SerialBT.println("Guardando Marca...");
        if (vioIzq) {
          marcaCuadradoDir[totalMarcasGuardadas] = -1;
        }  // Guarda la marca en el lado que vió
        if (vioDer) {
          marcaCuadradoDir[totalMarcasGuardadas] = +1;
        }
        totalMarcasGuardadas++;
      }

      Motor(40, 40);
      delay(140);
      Motor(0, 0);
      return;  // volver al PID
    } else {
      // SEMI sin línea → giro normal inmediato
      SerialBT.println("Giro 90...");
      Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
      delay(delayBase);
      puedeLaser = true;
      if (vioIzq) {
        girarCrudo(0);
        delay(333);
        giroWhile(0);
        Motor(0, 0);
        delay(delayBase);
        return;
      }
      if (vioDer) {
        girarCrudo(1);
        delay(333);
        giroWhile(1);
        Motor(0, 0);
        delay(delayBase);
        return;
      }
    }
  }

  // --- INTERSECCIÓN COMPLETA (ambos lados) ---
  if (vioIzq && vioDer) {
    if (!hayLineaFinal) {  // Si no hay linea delante

      /*
        if (distLab < distEntrance && !blockLabirint) {
        SerialBT.println("Laberinto...");
        Motor(velocidadBaseIzq - (velocidadBaseIzq - 10), velocidadBaseDer - (velocidadBaseDer - 10));
        delay(delayBase);
        labirint();
        digitalWrite(LED, LOW);
        return;
        }
      */
      if ((distLab < 1212) && (lab == false)) {
        SerialBT.println("Laberinto...");
        laberinto();
        digitalWrite(LED, LOW);
        unsigned long fia = millis();
        while ((millis() - fia) < 200) {
          bool nidpid = true;
          qtr.read(sensorValues);  // Lectura de los sensores de línea

          // Filtrado simple para el cálculo de posición
          for (uint8_t i = 0; i < SensorCount; i++) {
            if (sensorValues[i] < 4000) sensorValues[i] = 0;
          }

          // Posición manual ponderada
          uint32_t sumaPesada = 0;
          uint32_t sumaTotal = 0;
          for (uint8_t i = 0; i < SensorCount; i++) {
            sumaPesada += (uint32_t)sensorValues[i] * (i * 1000);
            sumaTotal += sensorValues[i];
          }
          uint16_t position = (sumaTotal > 0) ? (sumaPesada / sumaTotal) : 0;

          // ---- NUEVO: manejo de GAPS (todo blanco) ----
          if (sumaTotal == 0) {
            // Avanza recto con velocidad base para no “morir” en líneas segmentadas
            Motor(velocidadBaseIzq - 10, velocidadBaseDer - 10);
            // Evita acumular integral/derivativos inútiles
            integral = 0;
            lastError = 0;
            // No corremos PID en esta iteración
            nidpid = false;
          }

          // Seguimiento de línea normal
          if (nidpid) {
            PID(position);
          }
        }
        return;
      }
      if (totalMarcasGuardadas > 0) {  // Si hay marcas guardadas (Hay cuadrado)
        SerialBT.println("Cuadrado...");
        int dir = marcaCuadradoDir[0];

        // Desplazar las marcas para que la segunda pase a ser primera
        for (int i = 0; i < totalMarcasGuardadas - 1; i++) {
          marcaCuadradoDir[i] = marcaCuadradoDir[i + 1];
        }
        totalMarcasGuardadas--;

        // Ejecutar el giro
        Motor(velocidadBaseIzq - restaBase, velocidadBaseDer - restaBase);
        delay(delayBase);
        if (dir < 0) giroWhile(0);
        else giroWhile(1);

        forzarProximaSemi = true;  // Forzar la salida (En una semi (90°) con línea delante)
        inicioCuadrado = millis();
        return;
      } else {  // Si no hay marcas guardadas
        //No hay cuadrado, por lo tanto es el final
        SerialBT.println("¡Final!");
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
      SerialBT.println("Cruce...");
      Motor(40, 40);
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

  SerialBT.println("Nada concluyente...");

  Motor(velocidadBaseIzq - (velocidadBaseIzq - 10), velocidadBaseDer - (velocidadBaseDer - 10));
}
