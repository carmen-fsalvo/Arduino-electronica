
#include "arduinoFFT.h"

/* Parámetros de la señal */
const uint16_t samples = 1024;        // Número de muestras
const double samplingFrequency = 4000; // Frecuencia de muestreo (Hz)

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

  /* --- 3. Envío de datos por el Puerto Serie en formato texto --- */
  
  // Encabezado para la captura (opcional, ayuda a estructurar en MATLAB)
  Serial.println("--- INICIO CAPTURA ---");
  
  // Imprime 3 columnas: Índice | Señal_Tiempo | Magnitud_FFT
  for (uint16_t i = 0; i < (samples >> 1); i++)
  {
    double freq = (i * samplingFrequency) / samples;
    
    // Formato: Frecuencia (Hz) , Señal_Temporal , Magnitud_FFT
    Serial.print(freq, 2);
    Serial.print(",");
    Serial.print(vRaw[i], 4);
    Serial.print(",");
    Serial.println(vReal[i], 4);
  }

  Serial.println("--- FIN CAPTURA ---");

  delay(2000);  // Pausa de 2 segundos entre capturas para dar tiempo a grabar
}