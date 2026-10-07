#include "arduinoFFT.h"

/* Parámetros de la señal */
const uint16_t samples = 1024;          // Número total de muestras
const double samplingFrequency = 40000; // Frecuencia de muestreo (Hz)

unsigned int sampling_period_us;

/* Vectores */
double vReal[samples];
double vImag[samples];
double vRaw[samples];  // Copia de la señal original en el tiempo

const int micPin = A0;

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, samplingFrequency);

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  analogReadResolution(12);
  sampling_period_us = round(1000000.0 * (1.0 / samplingFrequency));
}

void loop()
{
  /* --- 1. Muestreo de la señal y guardado en vRaw --- */
  unsigned long microseconds;
  
  for (uint16_t i = 0; i < samples; i++)
  {
    microseconds = micros();

    double lectura = (double)analogRead(micPin) - 2048.0;

    vReal[i] = lectura;
    vRaw[i]  = lectura; 
    vImag[i] = 0.0;

    while ((micros() - microseconds) < sampling_period_us) {
      // Espera para mantener el período de muestreo constante
    }
  }

  /* --- 2. Cálculo de la FFT --- */
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();

  /* --- 3. Envío de datos por el Puerto Serie (1024 filas para CoolTerm) --- */
  
  Serial.println("--- INICIO CAPTURA ---");
  
  // Recorremos las 1024 muestras completas
  for (uint16_t i = 0; i < samples; i++)
  {
    double freq = (i * samplingFrequency) / samples;
    
    // Columna 1: Frecuencia (Hz)
    Serial.print(freq, 2);
    Serial.print(",");
    
    // Columna 2: Señal_Temporal (Conserva las 1024 muestras reales)
    Serial.print(vRaw[i], 4);
    Serial.print(",");
    
    // Columna 3: Magnitud_FFT
    // Hasta la frecuencia de Nyquist (muestra 511) envía la magnitud real de la FFT.
    // De la muestra 512 a la 1023 envía 0.0 para mantener la estructura de 3 columnas sin repetir simetría.
    if (i < (samples / 2)) {
      Serial.println(vReal[i], 4);
    } else {
      Serial.println(0.0);
    }

    // Pequeña pausa para no saturar el buffer USB nativo de la Arduino Giga
    delayMicroseconds(150); 
  }

  Serial.println("--- FIN CAPTURA ---");
  Serial.flush(); // Garantiza que se envíen todos los datos antes de pausar

  delay(4000);  // Pausa de 2 segundos entre capturas
}
