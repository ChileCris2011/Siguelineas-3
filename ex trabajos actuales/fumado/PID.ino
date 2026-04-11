void PID() {
  int error = position - 3500;
  integral += error;
  int derivative = error - lastError;
  int output = (Kp * error) + (Ki * integral) + (Kd * derivative);
  lastError = error;

  int vIzq = constrain(velBaseIzq + output, 0, 255);
  int vDer = constrain(velBaseDer - output, 0, 255);
  Motor(vIzq, vDer);
}


void filtrado() {
  for (uint8_t i = 0; i < SensorCount; i++) {
    if (sensorValues[i] < umbral) sensorValues[i] = 0;  // Así aseguramos que el valor sea distinto de cero solo cuando detecte la línea (de acuerdo al umbral)
  }
}

void posicionar() {
  uint32_t sumaPesada = 0;
  uint32_t sumaTotal = 0;

  for (uint8_t i = 0; i < SensorCount; i++) {
    sumaPesada += (uint32_t)sensorValues[i] * (i * 1000);
    sumaTotal += sensorValues[i];
  }

  position = (sumaTotal > 0) ? (sumaPesada / sumaTotal) : -1;

  /* Pequeña explicación:
    Para el cálculo de posición se usa la fórmula:

      v₀*0+v₁*1000+... / v₀+v₁

        sumaPesada     / sumaTotal

    Donde vₙ es el valor del sensor n.
    Ésto devuelve la posición de la siguiente manera:
    0, si el sensor 0 (v₀) detecta línea; 1000 si el sensor 1 (v₁) detecta línea y así...

    Ahora los cambios:
    Anteriormente se verificaba que sumaTotal sea mayor a 0 (todos los sensores sin línea)
    para evitar el resultado de la división por 0 (sumaPesada y sumaTotal serían cero, por lo que el resultado es indefinido (puede ser un valor al azar, el menor/mayor posible, null, NaN, etc. No sabemos el valor que puede obtener y el robot hace kbum) ).
    Si bien ésto está bien, el resultado que se le da es 0, el mismo que cuando se activa el sensor 0.
    Ésto hace que, en el manejo de gaps, uno quiera que el resultado sea 0 para iniciar con éste manejo.
    Como dijimos ántes, 0 también es el resultado de el sensor 0 con línea así que saltamos ese valor.
    Aquí tenemos 2 opciones:
    - Mantener la posición anterior
    - Cambiar position a, por ejemplo, -1; un valor que no se usa.
    Se opta por cambiar position a -1 cuando no hay línea para mantener la lógica del manejo de gaps.
    Mantener la posición anterior también es viable, ya que así el PID sigue actuando en caso de que se pierda la línea.

    ¿Ésto afecta en algo? No... No debería afectar tanto...
    Entonces... ¿por qué? ... Porque sí, ya sabes que me gusta complicarme...

    En conclusión,  si la línea está a la izquierda: 0
                    si la línea está al centro: 3500
                    si la línea está a la derecha: 7000
                    si no hay línea, -1. 
  */
}