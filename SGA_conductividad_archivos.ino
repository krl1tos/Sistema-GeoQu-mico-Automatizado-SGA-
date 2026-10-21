// ============================================================
//  SGA_sensores.ino
//  Programa principal: mide conductividad, temperatura/humedad del aire
//  y humedad del suelo.
//
//  Archivos de la carpeta:
//    SensorConductividad.h / .cpp : sensores de conductividad
//    SensorAmbiente.h / .cpp      : sensor DHT (temperatura y humedad)
//    SensorHumedadSuelo.h / .cpp  : sensor de humedad en suelo
//
//  Calibración de conductividad: con los tornillos de un sensor en una
//  solución de conductividad conocida, escribir en el Monitor Serie
//  (9600 baudios, "Nueva línea"):  número_de_sensor  conductividad_uS/cm
//  Ejemplo:                        1 1413
//
//  Calibración de humedad de suelo (dos puntos):
//    s 1   -> con el sensor 1 al aire (seco): guarda el punto 0 %
//    m 1   -> con el sensor 1 en agua (mojado): guarda el punto 100 %
// ============================================================

#include "SensorConductividad.h"
#include "SensorAmbiente.h"
#include "SensorHumedadSuelo.h"

const unsigned long INTERVALO_MS = 5000;   // Tiempo entre mediciones (ms)
int contador = 0;

// ---------- Sensores de conductividad ----------
// Formato: SensorConductividad("nombre", pinExcitacion, pinLectura, R1, K)

const int NUM_SENSORES_COND = 1;

SensorConductividad sensores[NUM_SENSORES_COND] = {
  SensorConductividad("deposito_grifo", 2, A0, 1000.0, 35.81)
  // Cuando esté conectado el segundo sensor:
  // , SensorConductividad("deposito_sal", 2, A1, 1000.0, 35.81)
};


// ---------- Sensor de temperatura y humedad ----------
// Formato: SensorAmbiente("nombre", pinDatos, tipo)
// tipo: DHT11 (azul, más común) o DHT22 (blanco)

SensorAmbiente ambiente("ambiente", 4, DHT11);


// ---------- Sensores de humedad de suelo ----------
// Formato: SensorHumedadSuelo("nombre", pinLectura, valorSeco, valorMojado)
// Opcional: un quinto dato con el pin que alimenta el sensor (para
// sensores resistivos), por ejemplo:  ("maceta", A2, 600, 250, 7)
// Los valores seco/mojado son orientativos: calibrarlos con "s 1" y "m 1"
// y copiar luego los valores que muestre el Monitor Serie.

const int NUM_SENSORES_SUELO = 1;

SensorHumedadSuelo suelos[NUM_SENSORES_SUELO] = {
  SensorHumedadSuelo("suelo", A7, 600.0, 270.0, 6)
};

// ---------- Programa ----------

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < NUM_SENSORES_COND; i++) {
    sensores[i].iniciar();
  }
  ambiente.iniciar();
  for (int i = 0; i < NUM_SENSORES_SUELO; i++) {
    suelos[i].iniciar();
  }

  Serial.println("Para calibrar conductividad: numero_de_sensor  conductividad_en_uS/cm");
  Serial.println("Ejemplo: 1 1413");
  Serial.println("Para calibrar suelo: s N (seco, al aire)  |  m N (mojado, en agua)");
  Serial.println("Ejemplo: s 1");
}


void loop() {
  contador++;
  // 1) Calibración, si llegó un comando por serie
  if (Serial.available() > 0) {
    char primero = Serial.peek();

    if (primero == 's' || primero == 'S' || primero == 'm' || primero == 'M') {
      // --- Calibración de humedad de suelo: "s N" o "m N" ---
      bool esSeco = (primero == 's' || primero == 'S');
      Serial.read();                                   // Consume la letra
      int numero = Serial.parseInt();
      while (Serial.available() > 0) Serial.read();    // Limpia el resto de la línea

      if (numero >= 1 && numero <= NUM_SENSORES_SUELO) {
        SensorHumedadSuelo &g = suelos[numero - 1];
        bool ok = esSeco ? g.calibrarSeco() : g.calibrarMojado();
        if (ok) {
          Serial.print("# ");
          Serial.print(g.nombre());
          Serial.print(esSeco ? " valor SECO = " : " valor MOJADO = ");
          Serial.print(esSeco ? g.valorSeco() : g.valorMojado(), 1);
          Serial.println("  (copiarlo en la lista de sensores de suelo)");
        } else {
          Serial.print("# Lectura invalida en ");
          Serial.print(g.nombre());
          Serial.println(": revisar conexion");
        }
      } else {
        Serial.println("# Comando no valido. Formato: s N  o  m N (ej: s 1)");
      }

    } else {
      // --- Calibración de conductividad: "sensor referencia" ---
      int numero = Serial.parseInt();
      float referencia = Serial.parseFloat();
      while (Serial.available() > 0) Serial.read();    // Limpia el resto de la línea

      if (numero >= 1 && numero <= NUM_SENSORES_COND && referencia > 0) {
        SensorConductividad &s = sensores[numero - 1];
        if (s.calibrar(referencia)) {
          Serial.print("# ");
          Serial.print(s.nombre());
          Serial.print(" calibrado. Nuevo K = ");
          Serial.print(s.k(), 2);
          Serial.println("  (copiarlo en la lista de sensores)");
        } else {
          Serial.print("# No se puede calibrar ");
          Serial.print(s.nombre());
          Serial.println(": electrodos fuera del agua");
        }
      } else {
        Serial.println("# Comando no valido. Formato: numero_de_sensor conductividad (ej: 1 1413)");
      }
    }
  }
  
  Serial.print(" -==={ Lectura N ");
  Serial.print(contador);
  Serial.println(" }===-");

  // 2) Medir y mostrar la conductividad
  for (int i = 0; i < NUM_SENSORES_COND; i++) {
    sensores[i].medir();

    Serial.print(sensores[i].nombre());
    Serial.print(" :: adc: ");
    Serial.print(sensores[i].adc(), 1);
    Serial.print(" // Vout: ");
    Serial.print(sensores[i].vout(), 3);
    Serial.print(" V // ");

    if (sensores[i].hayAgua()) {
      Serial.print("R2: ");
      Serial.print(sensores[i].resistencia(), 1);
      Serial.print(" ohm // Conductividad: ");
      Serial.print(sensores[i].conductividad(), 1);
      Serial.println(" uS/cm");
    } else {
      Serial.println("Sin agua (electrodos fuera o secos)");
    }
  }

  // 3) Medir y mostrar temperatura y humedad del aire
  ambiente.medir();
  Serial.print(ambiente.nombre());
  if (ambiente.lecturaValida()) {
    Serial.print(" :: Temperatura: ");
    Serial.print(ambiente.temperatura(), 1);
    Serial.print(" C // Humedad: ");
    Serial.print(ambiente.humedad(), 1);
    Serial.println(" %");
  } else {
    Serial.println(" :: Error de lectura (revisar conexion y tipo de sensor)");
  }

  // 4) Medir y mostrar la humedad del suelo
  for (int i = 0; i < NUM_SENSORES_SUELO; i++) {
    suelos[i].medir();

    Serial.print(suelos[i].nombre());
    if (suelos[i].lecturaValida()) {
      Serial.print(" :: adc: ");
      Serial.print(suelos[i].adc(), 1);
      Serial.print(" // Humedad suelo: ");
      Serial.print(suelos[i].humedad(), 1);
      Serial.println(" %");
    } else {
      Serial.println(" :: Error de lectura (sensor desconectado o en corto)");
    }
  }

  Serial.println("   ---======+======--- ");  
  Serial.println(); // Línea entre ciclos
  
  delay(INTERVALO_MS);
}
