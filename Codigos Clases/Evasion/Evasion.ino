#include <Wire.h>
#include <Adafruit_VL53L0X.h>

//#include <BluetoothSerial.h>

// Crea una instancia del sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//BluetoothSerial SerialBT;

const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(115200);
  //SerialBT.begin("Valor Absoluto de 3");

  Serial.println("Iniciando...");

  inicializarMotores();

  // Inicia la comunicacion I2C con el sensor
  if (!lox.begin()) {
    Serial.println(F("¡Error de inicio de V53L0X! Verifica las conexiones."));
    while (1)
      ;  // Detiene el programa si no se puede inicializar el sensor
  }

  Serial.println(F("VL53L0X iniciado correctamente"));
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;  //Declara la variable para almacenar los datos de la medición

  //Realiza la medición de distancia y almacena los resultados en la variable 'measure'
  lox.rangingTest(&measure, false);

  //Verifica si la medición es válida
  /*
  if (measure.RangeStatus != 4) {  // Si el estado es diferente a 4, la medición es válida
    Serial.print("Distancia: ");
    Serial.print(measure.RangeMilliMeter * 10);  // Imprime la distancia medida en centímetros
    Serial.println(" cm");
  } else {
    Serial.println("Fuera de rango");
  }

  delay(100);  // Espera 0.1 segundos entre lecturas para evitar sobrecargar el sistema
  */
  if (measure.RangeStatus != 4) {
    if (measure.RangeMilliMeter < 100) {
      Motor(0, 0);
      delay(100);
      Motor(0, -80);  //giro d
      delay(1500);
      Motor(0, 0);
      delay(100);
      /*
      Motor(50, 50);
      delay(500);
      Motor(80, -80);  //giro i
      delay(1500);
      Motor(0, 0);
      delay(2000);
      Motor(50, 50);
      delay(800);
      Motor(80, -80);  //giro i
      delay(1500);
      Motor(0, 0);
      delay(2000);
      Motor(50, 50);
      delay(500);
      Motor(-80, 80);  //giro d
      delay(1500);
      Motor(50, 50);
      delay(100);
      */
    }
  }
}