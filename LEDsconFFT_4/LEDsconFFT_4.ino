#include "arduinoFFT.h"
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <math.h>

/* --- Configuración de la Matriz MAX7219 (4 módulos de 8x8 = 32 columnas) --- */
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW // Cambia a GENERIC_HW si los LEDs se ven reflejados/invertidos
#define MAX_DEVICES 4                     // 4 módulos de 8x8 = 32 columnas

// Pines de conexión para Arduino Giga R1
#define DATA_PIN    11                    // COPI 11 (DIN)
#define CS_PIN      10                    // CS 10   (CS)
#define CLK_PIN     13                    // SCK 13  (CLK)

// Instanciación explícita por Software SPI para evitar bloqueos en placas ARM
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

/* --- Parámetros de la señal y FFT --- */
const uint16_t samples = 1024;          // Número de muestras
const double samplingFrequency = 40000; // Frecuencia de muestreo (Hz)

unsigned int sampling_period_us;

/* Vectores de cálculo para la FFT */
double vReal[samples];
double vImag[samples];

const int micPin = A0;

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, samplingFrequency);

/* --- Agrupación Logarítmica en Eje X (32 Bandas de Frecuencia) --- */
const uint16_t lowBin[32]  = { 0,  3,  4,  5,  6,  7,  8,  9, 11, 13, 15, 17, 20, 23, 26, 30, 34, 39, 45, 51, 58, 66, 75, 85, 97, 110, 125, 142, 161, 183, 208, 236};
const uint16_t highBin[32] = { 2,  3,  4,  5,  6,  7,  8, 10, 12, 14, 16, 19, 22, 25, 29, 33, 38, 44, 50, 57, 65, 74, 84, 96, 109, 124, 141, 160, 182, 207, 235, 307};

/* --- Parámetros Fijos de la Escala en Decibelios (dBFS) --- */
const double MAG_REF = 200000.0; // Referencia absoluta para 0 dBFS
const double DB_MIN  = -36.0;    // < -36 dBFS se considera ruido de fondo (0 o 1 LED)
const double DB_MAX  = 0.0;      // 0 dBFS representa la saturación / pico máximo (LED 8)

int bandValues[32]; // Almacena el número entero de LEDs encendidos (0 a 8) por columna

void setup()
{
  Serial.begin(115200);

  // Inicialización de la Matriz MAX7219
  mx.begin();
  mx.clear();
  mx.control(MD_MAX72XX::INTENSITY, 1); // Intensidad fija y baja (1 a 15)

  analogReadResolution(12);
  sampling_period_us = round(1000000.0 * (1.0 / samplingFrequency));
}

void loop()
{
  /* --- STEP 1: Muestreo de la señal desde el Micrófono (A0) --- */
  unsigned long microseconds;
  double sumaLecturas = 0.0;
  
  for (uint16_t i = 0; i < samples; i++)
  {
    microseconds = micros();

    double lectura = (double)analogRead(micPin);
    vReal[i] = lectura;
    vImag[i] = 0.0;
    sumaLecturas += lectura;

    while ((micros() - microseconds) < sampling_period_us) {
      // Garantiza el período de muestreo de 25 us (40 kHz)
    }
  }

  /* --- STEP 2: Eliminación de la Componente Continua (Offset DC) --- */
  double mediaDC = sumaLecturas / (double)samples;
  for (uint16_t i = 0; i < samples; i++) {
    vReal[i] -= mediaDC;
  }

  /* --- STEP 3: Cálculo de la FFT únicamente con Ventana de Hann --- */
  FFT.windowing(FFTWindow::Hann, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();

  /* --- STEP 4: Conversión Absoluta a dBFS y Asignación Fija a 8 LEDs --- */
  for (int b = 0; b < 32; b++) {
    double sumaBand = 0.0;
    int binesContados = 0;
    
    // Suma la magnitud ignorando el bin 0 (DC residual puro)
    for (int i = lowBin[b]; i <= highBin[b]; i++) {
      if (i == 0) continue; 
      sumaBand += vReal[i];
      binesContados++;
    }
    
    if (binesContados == 0) binesContados = 1;
    double magnitudMedia = sumaBand / binesContados;

    // Cálculo absoluto del nivel en dBFS respecto a la referencia fija
    double db = 20.0 * log10((magnitudMedia + 1e-6) / MAG_REF);

    // Mapeo absoluto: por debajo de DB_MIN (-36 dB) da 0 LEDs encendidos
    int ledsAltos = 0;
    if (db >= DB_MIN) {
      ledsAltos = map((int)db, (int)DB_MIN, (int)DB_MAX, 1, 8);
      ledsAltos = constrain(ledsAltos, 0, 8);
    }

    bandValues[b] = ledsAltos; 
  }

  /* --- STEP 5: Representación Gráfica en la Matriz de LEDs MAX7219 --- */
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.clear();

  for (int col = 0; col < 32; col++) {
    int ledsAltos = bandValues[col];

    uint8_t patronColumna = 0;
    for (int row = 0; row < ledsAltos; row++) {
      patronColumna |= (1 << row);
    }

    mx.setColumn(col, patronColumna);
  }

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON); 
  mx.control(MD_MAX72XX::SHUTDOWN, MD_MAX72XX::OFF);

  delay(20); 
}
