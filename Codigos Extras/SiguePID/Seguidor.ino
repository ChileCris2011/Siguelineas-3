//Función de manejo de intersecciones
void interseccion() {
  Motor(50, 50);
  delay(500);  // Avanza para leer adelante
  Motor(0, 0);
  qtr.read(sensorValues);
  if (sensorValues[3] != 0 && sensorValues[4] != 0) {  // Hay blanco después de la intersección
    if (primerCuadrado == -1 && segundoCuadrado == -1) {             // Si no tenemos ningúna marca cuadrada (entonces no hay más cuadrados)
      Motor(0, 0);
      for (int i = 0; i < 3; i++) {  // Se detiene y hace el juego de luces (Meta)
        delay(200);
        digitalWrite(LED, LOW);
        delay(200);
        digitalWrite(LED, HIGH);  // Si, es a propósito para dejar el led encendido
      }
      while (digitalRead(BOTON) == 0) {}  // Espera al botón para seguir si es necesario
      delay(500);
      loop();  // Para evitar seguir con la función
    }
    // (No pasará aqui si ya llegó a la meta, así que nos ahorramos un else)

    // Aquí es que estamos entrando hacia un cuadrado
    if (primerCuadrado >= 0) {  // Si se ha guardado una primera marca
      girar(primerCuadrado);
      inCuadrado = primerCuadrado;  // Gira hacia donde indica la marca y se guarda por donde se entró

    } else {  // Si no hay primera marca y antes ya se verificó que ninguna esté vacía, entonces hay segunda marca
      girar(segundoCuadrado);
      inCuadrado = segundoCuadrado;  // Gira hacia donde indica segunda la marca y se guarda por donde se entró
    }
  } else {
    // canRead = true; // Línea de indicación. Si no hay ningún (otro) hito delante, probablemente es para evasión
  }
}

//Función de manejo de giros de 90°
void giros(int direccion) {
  // Para verificar si no se ha leído primero un giro en vez de una intersección (por si llegamos chuecos)
  for (int i = 0; i < 250; i++) {
    if (direccion == 1) {  // Si la marca es a la derecha, se acomoda un poco a la derecha
      Motor(50, 0);
      qtr.read(sensorValues);             // Se lee el sensor del extremo izquierdo
      if (sensorValues[0] == 0) {  // Efectivamente, era una intersección
        interseccion();                   // Se maneja como intersección
        return;                           // Vuelve al código principal
      }
    } else {  // Si la marca es a la izquierda, se acomoda un poco a la izquierda
      Motor(0, 50);
      qtr.read(sensorValues);             // Se lee el sensor del extremo derecho
      if (sensorValues[7] == 0) {  // Efectivamente, era una intersección
        interseccion();                   // Se maneja como intersección
        return;                           // Vuelve al código principal
      }
    }
  }  // Si aún sigue aquí, es que no era una intersección
  Motor(50, 50);
  delay(500);  // Avanza para leer adelante
  Motor(0, 0);
  qtr.read(sensorValues);
  if (sensorValues[3] != 0 && sensorValues[4] != 0) {  // Si no hay nada (blanco)

    girar(direccion);  // Entonces no hay (o no deberia haber) problema en girar nada mas

  } else {  // Hay alguna línea delante

    // (Los saltos de línea son para la indentación de los comentarios)

    if (inCuadrado == -1) {  // Si no estamos dentro de un cuadrado
      // entonces se indica una marca
      if (primerCuadrado >= 0) {
        segundoCuadrado = direccion;  // Si ya guardamos una, se guarda en la segunda
      } else {
        primerCuadrado = direccion;  // Sino, se guarda en la primera
      }
    } else {  // Si estamos en un cuadrado, entonces es una salida
      if (inCuadrado == direccion) {
        inCuadrado = -1;  // Si es la misma dirección por la que entramos, entonces la salida está bien
        girar(direccion);
      } else {  // Detectamos una linea al lado contrario por el que entramos
        // Muy improbable, pero ha sucedido al tener la salida en una esquina del cuadrado en vez de en medio de este
        Motor(50, 50);
        delay(500);  // En ese caso, se pasa de largo
        Motor(0, 0);
      }
    }
  }
}