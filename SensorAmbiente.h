// ============================================================
//  SensorAmbiente.h
//  Sensor de temperatura y humedad del aire (DHT11 / DHT22).
//
//  Requiere la librería "DHT sensor library" de Adafruit
//  (Programa > Incluir librería > Administrar bibliotecas).
//
//  Conexión del módulo de 3 pines:
//     + (VCC)        -> 5 V
//     OUT / S (DATA) -> pin digital (por ejemplo D4)
//     - (GND)        -> GND
// ============================================================

#pragma once
#include <Arduino.h>
#include <DHT.h>       // Define también DHT11 y DHT22

class SensorAmbiente {
  public:
    // Constructor:
    //   nombre : texto para identificarlo en pantalla
    //   pin    : pin digital donde está conectado el pin de datos
    //   tipo   : DHT11 o DHT22 (según el sensor que tengas)
    SensorAmbiente(const char* nombre, byte pin, byte tipo);

    void iniciar();     // Inicia la comunicación con el sensor
    void medir();       // Lee temperatura y humedad

    // Consultas: devuelven los datos de la última medición válida.
    const char* nombre() { return _nombre; }
    float temperatura()  { return _temperatura; }   // °C
    float humedad()      { return _humedad; }       // % de humedad relativa
    bool lecturaValida() { return _valida; }        // false si la última lectura falló

  private:
    const char* _nombre;
    DHT _dht;              // Objeto de la librería que habla con el sensor
    float _temperatura;
    float _humedad;
    bool _valida;
};
