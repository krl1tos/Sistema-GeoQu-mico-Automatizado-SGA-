// ============================================================
//  SensorHumedadSuelo.h
//  Sensor de humedad en suelo con salida analógica
//  (capacitivo tipo v1.2, o resistivo tipo YL-69 / FC-28).
//
//  Conexión del módulo de 3 pines:
//     VCC  -> 5 V  (o a un pin digital si se usa pinAlimentacion)
//     AOUT -> pin analógico (por ejemplo A2)
//     GND  -> GND
//
//  Funcionamiento: el sensor entrega más tensión (más cuentas del ADC)
//  cuanto más SECO está el suelo, y menos cuanto más húmedo. Por eso hay
//  que calibrar dos puntos:
//     valorSeco   : lectura con el sensor en el aire / suelo seco
//     valorMojado : lectura con el sensor en agua / suelo saturado
//  y la humedad se calcula como un porcentaje entre ambos.
// ============================================================

#pragma once
#include <Arduino.h>

const int MUESTRAS_SUELO = 10;        // Lecturas que se promedian en cada medición
const byte SIN_PIN_ALIM = 255;        // Valor para "sensor siempre alimentado a 5 V"

// Rango de lectura válido (cuentas del ADC, 0 a 1023). Fuera de este rango
// se considera que el sensor está desconectado o en cortocircuito.
const float SUELO_ADC_MIN = 5;
const float SUELO_ADC_MAX = 1018;


class SensorHumedadSuelo {
  public:
    // Constructor:
    //   nombre        : texto para identificarlo en pantalla
    //   pinLectura    : pin analógico donde está conectado AOUT (A2, A3...)
    //   valorSeco     : cuentas del ADC con el sensor seco (al aire)
    //   valorMojado   : cuentas del ADC con el sensor en agua
    //   pinAlimentacion : (opcional) pin digital que alimenta el sensor solo
    //                     durante la medición. Útil con sensores resistivos,
    //                     que se corroen si están siempre alimentados.
    SensorHumedadSuelo(const char* nombre, byte pinLectura, float valorSeco, float valorMojado, byte pinAlimentacion = SIN_PIN_ALIM);

    void iniciar();             // Prepara los pines
    void medir();               // Mide y guarda los resultados

    // Calibración: se llama con el sensor en cada condición.
    // Devuelven false si la lectura no es válida.
    bool calibrarSeco();
    bool calibrarMojado();

    // Consultas: devuelven los datos de la última medición.
    const char* nombre()   { return _nombre; }
    float adc()            { return _adc; }
    float humedad()        { return _humedad; }       // % (0 a 100)
    float valorSeco()      { return _seco; }
    float valorMojado()    { return _mojado; }
    bool lecturaValida()   { return _valida; }

  private:
    float leerAdc();            // Promedio de varias lecturas

    const char* _nombre;
    byte _pinLec;
    byte _pinAlim;
    float _seco;
    float _mojado;

    // Resultados de la última medición.
    float _adc;
    float _humedad;
    bool _valida;
};
