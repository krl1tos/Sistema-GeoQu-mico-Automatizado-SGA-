// ============================================================
//  SensorConductividad.h
//  Declaración de la clase: qué datos tiene y qué sabe hacer.
//  El detalle de cómo lo hace está en SensorConductividad.cpp
//
//  Circuito de cada sensor:

//     pinExcitacion ---[ R1 ]---+--- Electrodo A
//                               |
//                          pinLectura (A0, A1...)
//
//     GND ----------------------- Electrodo B
//
// ============================================================

#pragma once          // Evita que el archivo se incluya dos veces
#include <Arduino.h>  // Necesario para usar byte, analogRead, etc.

// ---------- Configuración general (igual para todos los sensores) ----------

const float VIN = 5.0;       // Tensión de excitación (V)
const int MUESTRAS = 20;     // Lecturas que se promedian en cada medición

// Rango de lectura válido (cuentas del ADC, 0 a 1023). Fuera de este rango
// se considera que los electrodos están fuera del agua o en cortocircuito.
const float ADC_MIN = 5;
const float ADC_MAX = 1000;


// ---------- Clase ----------

class SensorConductividad {
  public:
    // Constructor: se ejecuta al crear cada sensor y recibe sus datos propios.
    //   nombre        : texto para identificarlo en pantalla
    //   pinExcitacion : pin digital que da 5 V (D2, D3...)
    //   pinLectura    : pin analógico donde se lee Vout (A0, A1...)
    //   r1            : resistencia fija de ese sensor (ohm)
    //   k             : constante de celda inicial (1/m)
    SensorConductividad(const char* nombre, byte pinExcitacion, byte pinLectura, float r1, float k);

    void iniciar();                          // Deja el pin de excitación apagado
    void medir();                            // Mide y guarda los resultados
    bool calibrar(float referenciaUsCm);     // Calcula K con una solución patrón

    // Consultas: devuelven los datos de la última medición.
    const char* nombre() { return _nombre; }
    float adc()          { return _adc; }
    float vout()         { return _vout; }
    float resistencia()  { return _r2; }
    float k()            { return _k; }
    bool hayAgua()       { return _hayAgua; }
    float conductividad();                   // En uS/cm

  private:
    float leerAdc();                         // Promedio de varias lecturas

    // Datos propios de cada sensor (el guion bajo indica "interno").
    const char* _nombre;
    byte _pinExc;
    byte _pinLec;
    float _r1;
    float _k;

    // Resultados de la última medición.
    float _adc;
    float _vout;
    float _r2;
    bool _hayAgua;
};
