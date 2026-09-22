
/*
  Ejemplo de uso de la librería arduinoFFT para calcular la FFT de una señal
  sinusoidal de una única frecuencia fija (2 Hz).

  Se muestra el espectro de magnitudes por el Serial Plotter.
*/

#include "arduinoFFT.h"

/* Parámetros de la señal */
const uint16_t samples = 1024;        // Debe ser potencia de 2
const double sampling = 40000;         // Frecuencia de muestreo (Hz)

const double signalFrequency = 113;   // Frecuencia de la señal a analizar (Hz)
const uint8_t amplitude = 4;

/* Vectores de entrada/salida */
double vReal[samples];
double vImag[samples];

/* Crear objeto FFT */
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, sampling);

void setup()
{
  Serial.begin(115200);
  while (!Serial);
}

void loop()
{
  /* --- 1. Construir la señal sinusoidal de 2 Hz --- */
  double ratio = twoPi * signalFrequency / sampling;  // Radianes por muestra
  for (uint16_t i = 0; i < samples; i++)
  {
    vReal[i] = int8_t(amplitude * sin(i * ratio) / 2.0);
    vImag[i] = 0;  // Reset del vector imaginario
  }

  /* --- 2. Calcular la FFT --- */
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();

  /* --- 3. Imprimir magnitudes para el Serial Plotter --- */
  // Solo la mitad del espectro (la otra mitad es simétrica)
  for (uint16_t i = 0; i < (samples >> 1); i++)
  {
    Serial.print(i);
    Serial.print(' ');
    Serial.println(vReal[i], 4);
  }

  delay(50);  // Pequeña pausa para que el plotter no se sature
}