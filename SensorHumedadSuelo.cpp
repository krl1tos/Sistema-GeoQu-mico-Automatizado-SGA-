// ============================================================
//  SensorHumedadSuelo.cpp
//  Implementación de la clase SensorHumedadSuelo.
//
//  Fórmula:  humedad (%) = (seco - adc) * 100 / (seco - mojado)
//  El resultado se limita al rango 0 - 100 %.
// ============================================================

#include "SensorHumedadSuelo.h"


// Constructor: guarda los datos que recibe en las variables internas.
SensorHumedadSuelo::SensorHumedadSuelo(const char* nombre, byte pinLectura,
                                       float valorSeco, float valorMojado,
                                       byte pinAlimentacion)
  : _nombre(nombre), _pinLec(pinLectura), _pinAlim(pinAlimentacion),
    _seco(valorSeco), _mojado(valorMojado),
    _adc(0), _humedad(0), _valida(false) {}


// Si se usa pin de alimentación, lo deja como salida y apagado.
void SensorHumedadSuelo::iniciar() {
  if (_pinAlim != SIN_PIN_ALIM) {
    pinMode(_pinAlim, OUTPUT);
    digitalWrite(_pinAlim, LOW);
  }
}


// Hace una medición y guarda los resultados dentro del objeto.
void SensorHumedadSuelo::medir() {
  _adc = leerAdc();
  _valida = (_adc > SUELO_ADC_MIN && _adc < SUELO_ADC_MAX);

  if (_valida && _seco != _mojado) {
    _humedad = (_seco - _adc) * 100.0 / (_seco - _mojado);
    _humedad = constrain(_humedad, 0.0, 100.0);
  } else {
    _humedad = 0;
  }
}


// Mide y guarda la lectura actual como "punto seco" (0 %).
bool SensorHumedadSuelo::calibrarSeco() {
  _adc = leerAdc();
  if (_adc <= SUELO_ADC_MIN || _adc >= SUELO_ADC_MAX) return false;
  _seco = _adc;
  return true;
}


// Mide y guarda la lectura actual como "punto mojado" (100 %).
bool SensorHumedadSuelo::calibrarMojado() {
  _adc = leerAdc();
  if (_adc <= SUELO_ADC_MIN || _adc >= SUELO_ADC_MAX) return false;
  _mojado = _adc;
  return true;
}


// Lee el pin varias veces y devuelve el promedio.
// Si hay pin de alimentación, solo se enciende mientras se mide.
float SensorHumedadSuelo::leerAdc() {
  if (_pinAlim != SIN_PIN_ALIM) {
    digitalWrite(_pinAlim, HIGH);
    delay(10);                     // Espera a que se estabilice
  }

  // Detección de sensor desconectado: con la resistencia pull-up interna
  // activada, un pin sin nada conectado sube a ~1023. Un sensor conectado
  // fija su propia tensión y la lectura queda por debajo.
  pinMode(_pinLec, INPUT_PULLUP);
  delay(5);
  analogRead(_pinLec);
  int prueba = analogRead(_pinLec);
  pinMode(_pinLec, INPUT);         // Vuelve a entrada normal

  if (prueba >= SUELO_ADC_MAX) {
    if (_pinAlim != SIN_PIN_ALIM) digitalWrite(_pinAlim, LOW);
    return 1023;                   // Se interpreta como "sin sensor" (lectura inválida)
  }

  analogRead(_pinLec);             // Primera lectura: se descarta

  long suma = 0;
  for (int i = 0; i < MUESTRAS_SUELO; i++) {
    suma += analogRead(_pinLec);
    delay(2);
  }

  if (_pinAlim != SIN_PIN_ALIM) {
    digitalWrite(_pinAlim, LOW);
  }
  return suma / (float)MUESTRAS_SUELO;
}
