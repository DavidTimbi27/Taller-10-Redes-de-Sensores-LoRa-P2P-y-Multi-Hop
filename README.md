# Taller 10: Redes de Sensores LoRa – P2P y Multi-Hop

![LoRa](https://img.shields.io/badge/LoRa-915MHz-blue)
![Platform](https://img.shields.io/badge/Plataforma-Heltec_V3-orange)
![Framework](https://img.shields.io/badge/Framework-Arduino_IDE-00979D)
![Universidad](https://img.shields.io/badge/Universidad-Universidad_de_Cuenca-red)

Este repositorio contiene el código fuente y la documentación para la implementación de una red de sensores inalámbricos de largo alcance utilizando la tecnología **LoRa** en topología **Punto a Punto (P2P)**. 

El proyecto fue desarrollado como parte de la asignatura de **Redes Inalámbricas de Sensores (WSN)** de la carrera de Ingeniería en Telecomunicaciones de la Universidad de Cuenca.

## Autores
* **Franklin Andrade** - [franklin.andrade@ucuenca.edu.ec]
* **David Timbi** - [david.timbi2702@ucuenca.edu.ec]

## Descripción del Proyecto
El objetivo principal de esta práctica es caracterizar el comportamiento del enlace de radio en la capa física de LoRa variando el **Factor de Esparcimiento (Spreading Factor - SF)**. Se configuraron dos nodos autónomos capaces de cambiar dinámicamente su SF (de SF7 a SF9 y luego a SF12) sobre la marcha para evaluar cómo afecta esto a la sensibilidad del receptor (RSSI, SNR) y al alcance máximo.

Las mediciones empíricas se realizaron en dos escenarios distintos:
1. **Campus Balzay (Entorno semi-abierto):** Mediciones a 50m, 100m y 300m.
2. **Av. Ordóñez Lazo (Entorno urbano denso):** Medición a 600m en condiciones de alto tráfico e interferencia.

## Requisitos de Hardware y Software

### Hardware
* 2x Placas de desarrollo **Heltec WiFi LoRa 32 V3** (Basadas en el microcontrolador ESP32-S3 y transceptor Semtech SX1262).
* Antenas de 915 MHz (incluidas con las placas).
* Baterías LiPo de 3.7V o cables USB-C para alimentación portátil.

### Software
* **Arduino IDE** (v2.x recomendado).
* **Heltec ESP32 Series Dev-boards** v3.0.3 (Instalable a través del Gestor de Tarjetas).
* Librería unificada de Heltec (se instala automáticamente con el core de la placa).

## Estructura del Repositorio

```text
├── TX_Robusto/
│   └── TX_Robusto.ino      # Código fuente del nodo transmisor (Publicador dinámico)
├── RX_Colector/
│   └── RX_Colector.ino     # Código fuente del nodo receptor (Procesador de telemetría)
├── imagenes/               # Evidencias fotográficas y gráficas de resultados (RSSI, SNR)
├── Informe_Taller10.pdf    # Reporte técnico completo en formato IEEE
└── README.md               # Este archivo
