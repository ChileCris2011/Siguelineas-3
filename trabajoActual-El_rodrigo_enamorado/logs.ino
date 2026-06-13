void updateLog(int actstat) {
  /*
  for (int i = 0; i < SensorCount; i++) {
    SerialBT.print(sensorValues[i]);
    if (i < 7) {
      SerialBT.print("|");
    }
  }

  SerialBT.print("/");
  SerialBT.print(actstat);
  SerialBT.print("/");
  SerialBT.print(lastMark);
  SerialBT.print("/");
  SerialBT.print(marcaCuadradoDir[0]);
  SerialBT.print("|");
  SerialBT.print(marcaCuadradoDir[1]);
  SerialBT.print("/");
  SerialBT.println(lecturaMM);
  SerialBT.flush();
  delayMicroseconds(500);
  */
}
/**
  * @brief Logs debug messages to Bluetooth
  * 
  * @param message Message to send
  * @param st Format header?
  * @param ln Format end of line?
  */
void elog(String message, bool st, bool ln) {
  if (st) { SerialBT.print("*"); }
  SerialBT.print(message);
  if (ln) { SerialBT.print("*\n"); }
}

/**
  * @brief Logs debug messages to Bluetooth
  * 
  * @param message Value to send
  * @param st Format header?
  * @param ln Format end of line?
  */
void elog(int message, bool st, bool ln) {
  if (st) { SerialBT.print("*"); }
  SerialBT.print(message);
  if (ln) { SerialBT.print("*\n"); }
}

/**
  * @brief Logs error messages to Bluetooth
  * 
  * @param message Error message to send
  * @param st Format header?
  * @param ln Format end of line?
  */
void errorLog(String message, bool st, bool ln) {
  if (st) { SerialBT.print("**"); }
  SerialBT.print(message);
  Serial.print(message);

  if (ln) {
    SerialBT.print("**\n");
    Serial.print('\n');
  }
}

/**
  * @brief Logs error messages to Bluetooth
  * 
  * @param message Error value to send
  * @param st Format header?
  * @param ln Format end of line?
  */
void errorLog(int message, bool st, bool ln) {
  if (st) { SerialBT.print("**"); }
  SerialBT.print(message);
  Serial.print(message);

  if (ln) {
    SerialBT.print("**\n");
    Serial.print('\n');
  }
}