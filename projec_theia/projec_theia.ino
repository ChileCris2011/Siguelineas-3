#include <Adafruit_VL53L0X.h>

// Pines motores
#define AIN1 16
#define AIN2 17
#define PWMA 4
#define BIN1 5
#define BIN2 18
#define PWMB 19

// Botón
#define BOTON 12

#define LED 2

//Direcciones I2C L0X
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31
#define LOX3_ADDRESS 0x32

//Pines apagado L0X
#define SHT_LOX1 23
#define SHT_LOX2 14
#define SHT_LOX3 13

Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();

VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;
VL53L0X_RangingMeasurementData_t measure3;

const int freq = 5000;
const int resolution = 8;

void inicializarMotores() {
  pinMode(BIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  ledcSetup(0, freq, resolution);
  ledcAttachPin(PWMB, 0);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  ledcSetup(1, freq, resolution);
  ledcAttachPin(PWMA, 1);
}

void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  digitalWrite(SHT_LOX3, HIGH);
  delay(10);

  // activating LOX1 and resetting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);

  // initing LOX1
  if (!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while (1) digitalWrite(LED, HIGH);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if (!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while (1) digitalWrite(LED, HIGH);
  }

  // activating LOX3
  digitalWrite(SHT_LOX3, HIGH);
  delay(10);

  //initing LOX3
  if (!lox3.begin(LOX3_ADDRESS)) {
    Serial.println(F("Failed to boot third VL53L0X"));
    while (1) digitalWrite(LED, HIGH);
  }
}

void Motoriz(int value) {
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
// Función accionamiento motor derecho
void Motorde(int value) {
  if (value >= 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    value *= -1;
  }
  ledcWrite(0, value);
}

//Accionamiento de motores
void Motor(int left, int righ) {
  Motoriz(left);
  Motorde(righ);
}

// PID
float Kp = 0.25;
float Ki = 0;
float Kd = 0.8;

float error = 0;
float errorAnterior = 0;
float integral = 0;

unsigned long tiempoAnterior = 0;

int velocidadBase = 40;

int leerSensor(Adafruit_VL53L0X &sensor, VL53L0X_RangingMeasurementData_t &medicion) {

  sensor.rangingTest(&medicion, false);

  if (medicion.RangeStatus != 4)
    return medicion.RangeMilliMeter;

  return 2000;  // fuera de rango
}

void setup() {
  Serial.begin(115200);

  inicializarMotores();

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);
  pinMode(SHT_LOX3, OUTPUT);

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);

  delay(200);

  setID();

  delay(1000);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
}

int tolerance = 20;  //mm

void loop() {

  // Leer sensores laterales
  int izquierda = leerSensor(lox2, measure2);
  int derecha = leerSensor(lox3, measure3);

  int centro = leerSensor(lox1, measure1);

  int pos = izquierda - derecha;

  if (centro < 123) {
    Motor(0, 0);
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      delay(500);
    }
    while (true) {}
  }

  if (abs(pos) <= tolerance) {
    Motor(50, 50);
  } else if (pos > tolerance && pos <= tolerance + 30) {
    Motor(20, 60);
  } else if (pos < -tolerance && pos >= -tolerance - 30) {
    Motor(60, 20);
  } else if (pos > tolerance + 30) {
    Motor(0, 70);
  } else if (pos < -tolerance - 30) {
    Motor(70, 0);
  }
}