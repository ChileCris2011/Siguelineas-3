void inicializarMotores() {
  pinMode(BIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  ledcSetup(1, frecuencia, resolution);
  ledcAttachPin(PWMB, 1);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  ledcSetup(0, frecuencia, resolution);
  ledcAttachPin(PWMA, 0);
}

void Motoriz(int value) {
  if (value >= 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } else {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    value *= -1;
  }
  ledcWrite(0, value);
}
// Función accionamiento motor derecho
void Motorde(int value) {
  if (value >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    value *= -1;
  }
  ledcWrite(1, value);
}

// Accionamiento de motores
void Motor(int righ, int left) {
  Motoriz(left);
  Motorde(righ);
}

void girar(int direccion) {
  if (direccion == 1) {
    Motor(0, 0);
    delay(200);
    Motor(-80, 80);  // giro der
    delay(700);
    Motor(50, 50);
    delay(100);
  } else if (direccion == 0) {
    Motor(0, 0);
    delay(200);
    Motor(80, -80);  //giro izq
    delay(700);
    Motor(50, 50);
    delay(100);
  }
}

void interseccion() {
  Motor(50, 50);
  delay(500);
  Motor(0, 0);
  qtr.read(sensorValues);
  if (sensorVales[3] < umbral && sensorValues[4] < umbral) {
    if (primerCuadrado == -1 && segundoCuadrado == -1) {
      Motor(0, 0);
      while (true) {
        delay(200);
        digitalWrite(LED, HIGH);
        delay(200);
        digitalWrite(LED, LOW);
      }
    }
    if (primerCuadrado >= 0) {
      girar(primerCuadrado);
      inCuadrado = true;
    } else {
      girar(segundoCuadrado);
      inCuadrado = true;
    }
  }
}

void giros(int direccion) {
  /*
      for (int i = 0; i < 250; i++){
        Motor(0, 50);
        qtr.read(sensorValues);
        if(sensorValues[7] > umbral) {
          interseccion();
          return;
        }
      }
    */
  Motor(50, 50);
  delay(500);
  Motor(0, 0);
  qtr.read(sensorValues);
  if (sensorVales[3] < umbral && sensorValues[4] < umbral) {
    girar(direccion);
  } else {
    if (!inCuadrado) {
      if (primerCuadrado >= 0) {
        segundoCuadrado = direccion;
      } else {
        primerCuadrado = direccion;
      }
    } else {
      girar(direccion);
      inCuadrado = false;
    }
  }
}