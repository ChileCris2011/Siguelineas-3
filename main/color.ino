/*uint16_t* leerSensor(Adafruit_TCS34725& sensor, uint8_t canal) {
  muxSelect(canal);

  uint16_t r, g, b, c;
  sensor.getRawData(&r, &g, &b, &c);

  uint16_t listData[4] = { r, g, b, c };
  return listData;
}*/

void muxSelect(uint8_t canal) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << canal);
  Wire.endTransmission();
}
