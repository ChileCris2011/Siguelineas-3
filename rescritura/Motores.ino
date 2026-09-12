void iniciarMotores() {
  pinMode(BIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  if (!(ledcAttach(PWMA, freq, resolution) || ledcAttach(PWMB, freq, resolution))) {
    while (true) {
      blink(1000, 2000); // Patron de error al asignar canales
    }
  }
}

void Motor(int left, int right) {
  if (left >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    left *= -1;
  }
  if (right >= 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    right *= -1;
  }
  ledcWrite(PWMA, left);
  ledcWrite(PWMB, right);
}