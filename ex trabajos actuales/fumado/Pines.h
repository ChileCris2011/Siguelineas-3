
// Pines motores
#define AIN1 17
#define AIN2 16
#define PWMA 4
#define BIN1 5
#define BIN2 18
#define PWMB 19

// Botón
#define BOTON 12

#define LED 2

// Direcciones (SIGANLAS)
#define Izquierda -1
#define Derecha 1

// Estados
// Aqui definiré los estados de evaluación para ser guardados y saber cual es cual
// Pueden ser... entcuadrado, noventa, forzosemi... etc...

// Proyecto Manhattan ;)
// #define TRIG_PIN 14
// #define ECHO_PIN 13

// Bien, como el compilador se queja de que las funciones no han sido declaradas donde él quiere que estén:
/// Motores.ino
extern void inicializarMotores();
extern void Motor(int left, int righ);
/// Linea.ino
extern void inicializarQTR(bool calibrar = false);
extern void calibrarQTR(uint16_t t = 150);
extern void evaluarCruce();
/// Laser.ino
extern void inicializarLaser();
extern void evadir();
/// Gyro.ino
extern void inicializarGyro();
/// misc.ino
extern void blink(int rep, int del, int adel = -1);
extern void esperarBoton();
extern void inicio();
/// PID.ino
extern void PID();
extern void filtrado();
extern void posicionar();
/// Giros.ino
extern void Girar(float grados, int direction);
extern void girarCrudo(int direccion);
extern void giroWhile(int direccion, bool ver = false);
// Aunque pensandolo bien también sirve como un índice para ver donde está cada función y que valores toma.


/***********************************
Robot 2:
  AIN1 16
  AIN2 17
  PWMA 4
  BIN1 5
  BIN2 18
  PWMB 19

  /////////
  LOW
  HIGH

  HIGH
  LOW
  /////////

  Cable X (color) Y (posicion)
Robot 3:
  AIN1 17
  AIN2 16
  PWMA 4
  BIN1 5
  BIN2 18
  PWMB 19
  
  /////////
  HIGH
  LOW

  LOW
  HIGH
  /////////

  Cable Verde Adelante
************************************/