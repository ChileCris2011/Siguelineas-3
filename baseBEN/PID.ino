void pid(float vel_base, int lim, float KP, float KI, float KD, int inflim) {
  int posicion = qtr.readLineBlack(sensorValues);
  posicion = map(posicion, 0, 7000, -255, 255);  
  proportional = posicion - ref;
  derivative = proportional - last_proportional;
  last_proportional = proportional;
  integrative = last_proportional + error2 + error3 + error4 + error5 + error6;
  error6 = error5;
  error5 = error4;
  error4 = error3;
  error3 = error2;
  error2 = last_proportional;
  int power_difference = (proportional * KP) + (derivative * KD) + (integrative * KI); 
  if (power_difference > lim)
    power_difference = lim;
  else if (power_difference < -lim)
    power_difference = -lim;
  int velizq = vel_base + power_difference;
  int velder = vel_base - power_difference;
  if (velder <= -inflim) {
    velder = -inflim;
  }
  if (velizq <= -inflim) {
    velizq = -inflim;
  }
  SerialBT.print(posicion);  
  SerialBT.print("  ");
  SerialBT.print(velizq);
  SerialBT.print("  ");
  SerialBT.println(velder);      
  mover(velizq, velder);
}