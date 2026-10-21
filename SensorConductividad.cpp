// ============================================================
//  SensorConductividad.cpp
//  Implementación de la clase: acá está el código de cada función.
//
//  Fórmulas:   Vout = adc * 5 / 1023
//              R2   = Vout * R1 / (5 - Vout)    (R2 = resistencia del agua)
//              σ    = K / R2                    (K = constante de celda)
//              1 S/m = 10000 uS/cm
// ============================================================

#include "SensorConductividad.h"

// "SensorConductividad::" indica que cada función pertenece a la clase.


// Constructor: guarda los datos que recibe en las variables internas.
SensorConductividad::SensorConductividad(const char* nombre, byte pinExcitacion,byte pinLectura, float r1, float k)  : _nombre(nombre), _pinExc(pinExcitacion), _pinLec(pinLectura), _r1(r1), _k(k), _adc(0), _vout(0), _r2(0), _hayAgua(false) {}

// Deja el pin de excitación como salida y apagado.
void SensorConductividad::iniciar() {
  pinMode(_pinExc, OUTPUT);
  digitalWrite(_pinExc, LOW);
}


// Hace una medición y guarda los resultados dentro del objeto.
void SensorConductividad::medir() {
  _adc = leerAdc();
  _vout = _adc * VIN / 1023.0;
  _hayAgua = (_adc > ADC_MIN && _adc < ADC_MAX);

  if (_hayAgua) {
    _r2 = _vout * _r1 / (VIN - _vout);
  } else {
    _r2 = 0;
  }
}


// Mide y calcula K a partir de una conductividad de referencia (uS/cm).
// Devuelve false si los electrodos están fuera del agua.
bool SensorConductividad::calibrar(float referenciaUsCm) {
  medir();
  if (!_hayAgua) return false;
  _k = (referenciaUsCm / 10000.0) * _r2;     // K = σ (S/m) * R2
  return true;
}


// Conductividad de la última medición, en uS/cm.
float SensorConductividad::conductividad() {
  if (!_hayAgua) return 0;
  return (_k / _r2) * 10000.0;               // S/m -> uS/cm
}


// Enciende la excitación, lee el pin varias veces y devuelve el promedio.
// La excitación solo está encendida mientras se mide, para no corroer
// los tornillos.
float SensorConductividad::leerAdc() {
  digitalWrite(_pinExc, HIGH);
  delay(10);                       // Espera a que se estabilice

  analogRead(_pinLec);             // Primera lectura: se descarta

  long suma = 0;
  for (int i = 0; i < MUESTRAS; i++) {
    suma += analogRead(_pinLec);
    delay(2);
  }

  digitalWrite(_pinExc, LOW);
  return suma / (float)MUESTRAS;
}
