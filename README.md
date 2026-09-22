# Analizador Espectral de Sonido en Tiempo Real

Este repositorio contiene el código fuente, pruebas de concepto y desarrollos para la implementación de un Analizador Espectral de Sonido en tiempo real, desarrollado dentro de la asignatura de Técnicas Experimentales Avanzadas en el Grado en Física (UAM)[cite: 2, 9].

## Descripción del Proyecto
El sistema captura audio analógico a través de un micrófono, realiza el procesamiento digital de la señal mediante algoritmos de Transformada Rápida de Fourier (FFT) para descomponer el espectro de frecuencias según el Teorema de Nyquist, y representa visualmente las bandas de frecuencia en una matriz de LEDs.

## Hardware Utilizado
* **Placa de desarrollo:** Arduino Giga R1 WiFi
* **Entrada de audio:** Módulo de micrófono analógico
* **Salida de audio / Generación de tono:** Altavoz piezoeléctrico[cite: 9]
* **Visualización:** Matriz de LEDs para el espectro de frecuencias[cite: 9]

## Estructura del Repositorio y Módulos
* **`pruebaMicroClase_Nyquist/`:** Muestreo de señal analógica aplicando criterios de la frecuencia de Nyquist para evitar el fenómeno de *aliasing*[cite: 9].
* **`pruebaFFT_1/`:** Implementación del algoritmo FFT (*Fast Fourier Transform*) para la descomposición espectral de la señal de audio en tiempo real[cite: 9].
* **`pruebaMicro/`:** Algoritmos de lectura y acondicionamiento de señal desde el micrófono[cite: 9].
* **`pruebaTone/` y `arregloPruebaTone/`:** Generación de tonos de prueba y frecuencias de referencia para la calibración del sistema[cite: 9].
* **`coolterm_archivo1.cooltermsettings`:** Configuración de comunicación serie mediante CoolTerm para el monitoreo y registro de datos[cite: 9].

## Entorno de Desarrollo y Librerías
* **IDE:** Arduino IDE / Visual Studio Code
* **Lenguaje:** C/C++
* **Herramientas de Comunicación:** CoolTerm (Serial Monitoring)[cite: 9]
