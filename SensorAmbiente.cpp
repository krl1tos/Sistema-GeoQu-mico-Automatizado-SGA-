// ============================================================
//  SensorAmbiente.cpp
//  Implementación de la clase SensorAmbiente.
// ============================================================

#include "SensorAmbiente.h"


// Constructor: guarda los datos y crea el objeto DHT de la librería.
SensorAmbiente::SensorAmbiente(const char* nombre, byte pin, byte tipo)
  : _nombre(nombre), _dht(pin, tipo),
    _temperatura(0), _humedad(0), _valida(false) {}


// Inicia la comunicación con el sensor. Se llama una vez en setup().
void SensorAmbiente::iniciar() {
  _dht.begin();
}


// Lee temperatura y humedad.
// Si la lectura falla (cable suelto, sensor mal conectado), la librería
// devuelve NaN ("no es un número"); en ese caso se conservan los últimos
// valores buenos y se marca la lectura como no válida.
void SensorAmbiente::medir() {
  float t = _dht.readTemperature();   // °C
  float h = _dht.readHumidity();      // % humedad relativa

  _valida = !(isnan(t) || isnan(h));

  if (_valida) {
    _temperatura = t;
    _humedad = h;
  }
}
