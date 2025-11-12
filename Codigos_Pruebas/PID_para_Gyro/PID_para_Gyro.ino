#include "Pines.h"
#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

// PID variables
float kp = 2.0;   // Proporcional
float ki = 0.0;   // Integral
float kd = 0.5;   // Derivativa

float error, previous_error = 0;
float integral = 0;
float derivative;
float pid_output;

float target_angle = 0;  // Ángulo objetivo (recto)
unsigned long last_time;

const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  mpu.begin();
  Serial.println("MPU iniciado, calibrando...");
  delay(1000);
  mpu.calcGyroOffsets();  // Calibra con quietud

  inicializarMotores();

  // Tiempo inicial
  last_time = millis();
}

void loop() {
  mpu.update();

  // Lee ángulo Z (yaw)
  float current_angle = mpu.getAngleZ();

  // Calcula error
  error = target_angle - current_angle;

  // Tiempo transcurrido
  unsigned long current_time = millis();
  float dt = (current_time - last_time) / 1000.0;

  // PID
  integral += error * dt;
  derivative = (error - previous_error) / dt;
  pid_output = kp * error + ki * integral + kd * derivative;

  previous_error = error;
  last_time = current_time;

  // Velocidades base
  int right_speed = 0 + pid_output;
  int left_speed = 0 - pid_output;

  // Limita valores entre 0 y 255
  left_speed = constrain(left_speed, -255, 255);
  right_speed = constrain(right_speed, -255, 255);

  Motor(left_speed, right_speed);

  // Debug
  Serial.print("Ángulo Z: ");
  Serial.print(current_angle);
  Serial.print(" | Error: ");
  Serial.print(error);
  Serial.print(" | L: ");
  Serial.print(left_speed);
  Serial.print(" | R: ");
  Serial.println(right_speed);

  delay(10);  // Ajusta si es necesario
}
