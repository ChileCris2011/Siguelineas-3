void iniciarLaser() {
  if (!lox.begin()) {
    Serial.println(F("Error de Laser"));
    while (1) {
      blink(1000, 1000);
      blink(1000, 1000);  // Patron error laser
      blink(500, 500);    // L - L - C - C - C
      blink(500, 500);
      blink(500, 2000);
    }
  }
  lox.startRangeContinuous();  // Empieza la lectura contínua del láser
}

void detectarObstaculo() {
  if (obstaculo && lox.isRangeComplete()) {
    int lecturaMM = lox.readRange();
    Serial.print("Lox: ");
    Serial.println(lecturaMM);

    if (lecturaMM < distanciaObstaculo) {
      
    }
  }
}