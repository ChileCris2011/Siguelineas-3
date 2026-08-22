#include <Adafruit_TCS34725.h>
#include <Wire.h>

#define BOTON 12
#define LED 2

// Dirección I2C del multiplexor PCA9548A
#define MUXADDR 0x70

// Canales del multiplexor
#define CANAL_SENSOR_1 0
#define CANAL_SENSOR_2 1

// Ambos sensores usan la misma dirección I2C (0x29),
// pero al estar en canales distintos del mux no hay conflicto.
Adafruit_TCS34725 rgb1 = Adafruit_TCS34725(10, TCS34725_GAIN_1X);
Adafruit_TCS34725 rgb2 = Adafruit_TCS34725(10, TCS34725_GAIN_1X);

#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth no esta activado o la placa no lo contiene
#endif

BluetoothSerial SerialBT;

void muxSelect(uint8_t canal) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << canal);
  Wire.endTransmission();
}

void setup() {
  SerialBT.begin("|3|2");
  pinMode(LED, OUTPUT);
  pinMode(BOTON, INPUT);

  while (digitalRead(BOTON) == LOW) delay(10);

  Wire.begin();

  // --- Inicializar sensor 1 ---
  muxSelect(CANAL_SENSOR_1);
  if (rgb1.begin()) {  // Inicializa el TCS34725
    SerialBT.println("Found sensor 1");
  } else {
    SerialBT.println("No TCS34725 1 found ... check your connections");
    while (true) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(500);
    }
  }

  // --- Inicializar sensor 2 ---
  muxSelect(CANAL_SENSOR_2);
  if (rgb2.begin()) {  // Inicializa el TCS34725
    SerialBT.println("Found sensor 2");
  } else {
    SerialBT.println("No TCS34725 2 found ... check your connections");
    while (true) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);  // Detiene el programa si no se puede inicializar el sensor
      delay(500);
    }
  }
}

void leerSensor(Adafruit_TCS34725& sensor, uint8_t canal, const char* nombre) {
  muxSelect(canal);

  uint16_t r, g, b, c;
  sensor.getRawData(&r, &g, &b, &c);

  SerialBT.print(nombre);
  SerialBT.print(" -> R: ");
  SerialBT.print(r);
  SerialBT.print("  G: ");
  SerialBT.print(g);
  SerialBT.print("  B: ");
  SerialBT.print(b);
  SerialBT.print("  C: ");
  SerialBT.println(c);
}

void loop() {
  while (digitalRead(BOTON) == LOW) delay(10);
  leerSensor(rgb1, CANAL_SENSOR_1, "Sensor 1");
  leerSensor(rgb2, CANAL_SENSOR_2, "Sensor 2");

  SerialBT.println("-----------------------------");
  delay(500);
}
