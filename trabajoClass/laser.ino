Laser_ Laser;

void Laser_::begin() {
  // Configuración Laser
  if (!lox.begin()) {
    Serial.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1) {
      digitalWrite(LED, HIGH);
      delay(1000);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(500);
    }
  }
  lox.startRangeContinuous();  // Empieza la lectura contínua del láser
}