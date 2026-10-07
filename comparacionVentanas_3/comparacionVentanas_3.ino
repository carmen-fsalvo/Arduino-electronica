#include "arduinoFFT.h"

/* Parámetros de la señal */
const uint16_t samples = 1024;          // Número de muestras
const double samplingFrequency = 40000; // Frecuencia de muestreo (Hz)

unsigned int sampling_period_us;

/* Vector para almacenar la señal temporal original sin offset DC */
double vRawSinDC[samples];

/* Vectores de cálculo para la FFT (se reutilizan para cada ventana) */
double vReal[samples];
double vImag[samples];

/* Vectores donde guardaremos los resultados de la FFT (primeras 512 muestras) */
const uint16_t halfSamples = samples / 2;
double fft_Rect[halfSamples];
double fft_Hamm[halfSamples];
double fft_Hann[halfSamples];
double fft_Black[halfSamples];

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
  /* --- 1. Muestreo de la señal --- */
  unsigned long microseconds;
  double sumaLecturas = 0.0;
  
  for (uint16_t i = 0; i < samples; i++)
  {
    microseconds = micros();

    double lectura = (double)analogRead(micPin);
    vRawSinDC[i] = lectura;
    sumaLecturas += lectura;

    while ((micros() - microseconds) < sampling_period_us) {
      // Mantiene constante el período de muestreo
    }
  }

  /* --- 2. Eliminación de la Componente Continua (Offset DC) --- */
  double mediaDC = sumaLecturas / (double)samples;
  for (uint16_t i = 0; i < samples; i++) {
    vRawSinDC[i] -= mediaDC;
  }

  /* --- 3. FFT 1: Ventana Rectangular (Sin Ventana) --- */
  prepararVectores();
  FFT.windowing(FFTWindow::Rectangle, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();
  guardarResultadoFFT(fft_Rect);

  /* --- 4. FFT 2: Ventana Hamming --- */
  prepararVectores();
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();
  guardarResultadoFFT(fft_Hamm);

  /* --- 5. FFT 3: Ventana Hann --- */
  prepararVectores();
  FFT.windowing(FFTWindow::Hann, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();
  guardarResultadoFFT(fft_Hann);

  /* --- 6. FFT 4: Ventana Blackman --- */
  prepararVectores();
  FFT.windowing(FFTWindow::Blackman, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();
  guardarResultadoFFT(fft_Black);

  /* --- 7. Envío de datos por Puerto Serie (6 columnas CSV) --- */
  Serial.println("--- INICIO CAPTURA ---");
  
  for (uint16_t i = 0; i < halfSamples; i++)
  {
    double freq = (i * samplingFrequency) / samples;
    
    // Columna 1: Frecuencia (Hz)
    Serial.print(freq, 2);
    Serial.print(",");
    
    // Columna 2: Señal Temporal Original sin DC
    Serial.print(vRawSinDC[i], 4); 
    Serial.print(",");
    
    // Columna 3: FFT Rectangular
    Serial.print(fft_Rect[i], 4);
    Serial.print(",");

    // Columna 4: FFT Hamming
    Serial.print(fft_Hamm[i], 4);
    Serial.print(",");

    // Columna 5: FFT Hann
    Serial.print(fft_Hann[i], 4);
    Serial.print(",");

    // Columna 6: FFT Blackman
    Serial.println(fft_Black[i], 4);

    delayMicroseconds(150); 
  }

  Serial.println("--- FIN CAPTURA ---");
  Serial.flush(); 

  delay(4000);  
}

/* --- Funciones Auxiliares --- */

// Copia la señal limpia original y reinicia la parte imaginaria a 0
void prepararVectores() {
  for (uint16_t i = 0; i < samples; i++) {
    vReal[i] = vRawSinDC[i];
    vImag[i] = 0.0;
  }
}

// Guarda la mitad útil de la FFT (hasta la frecuencia de Nyquist)
void guardarResultadoFFT(double* destino) {
  for (uint16_t i = 0; i < halfSamples; i++) {
    destino[i] = vReal[i];
  }
}