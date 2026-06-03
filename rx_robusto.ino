#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

#define RF_FREQUENCY           927100000 
#define LORA_BANDWIDTH         0         
#define LORA_CODINGRATE        1         
#define LORA_PREAMBLE_LENGTH   8         
#define LORA_SYMBOL_TIMEOUT    0         
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON   false     

extern SSD1306Wire display;
static RadioEvents_t RadioEvents;

int16_t lastRSSI = 0;
int8_t lastSNR = 0;
String rxPacket = "Ninguno";
String estadoRx = "Escuchando...";
unsigned long tiempoUltimoDato = 0;
int currentSF = 7; 
bool testCompleted = false;

void setup() {
  Serial.begin(115200);
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(100);

  display.init();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 0, "Iniciando RX Robusto...");
  display.display();

  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  RadioEvents.RxDone = OnRxDone;
  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);
  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, currentSF,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, false, true);
  Radio.Rx(0);
}

void loop() {
  Radio.IrqProcess();
  
  if (!testCompleted && estadoRx == "¡Dato recibido!" && (millis() - tiempoUltimoDato > 1500)) {
    estadoRx = "Escuchando...";
  }

  uint16_t bateriaMv = BoardGetBatteryVoltage();
  float bateriaVoltios = bateriaMv / 1000.0;

  display.clear();
  display.drawString(0, 0, "NODO RX Robusto - 927.1");
  
  if (testCompleted) {
    display.drawString(0, 13, "ESTADO: ENVIO COMPLETADO");
    display.drawString(0, 26, "Último RSSI: " + String(lastRSSI) + "dBm");
    display.drawString(0, 39, "Último SNR: " + String(lastSNR) + "dB");
  } else {
    display.drawString(0, 13, "Est: " + estadoRx + " (SF " + String(currentSF) + ")");
    display.drawString(0, 26, "Dato: " + rxPacket);
    display.drawString(0, 39, "RSSI: " + String(lastRSSI) + "dBm | SNR: " + String(lastSNR));
  }
  display.drawString(0, 52, "Bat: " + String(bateriaVoltios, 2) + "V (" + String(bateriaMv) + "mV)");
  display.display();
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
  lastRSSI = rssi;
  lastSNR = snr;
  tiempoUltimoDato = millis();
  
  rxPacket = "";
  for (int i = 0; i < size; i++) rxPacket += (char)payload[i];
  
  Serial.printf("Recibido: %s | RSSI: %d dBm\n", rxPacket.c_str(), lastRSSI);

  // EVALUAR COMANDOS DE RECONFIGURACIÓN
  if (rxPacket == "Cambio SF 9") {
    estadoRx = "¡ALERTA: CAMBIO A SF 9!";
    currentSF = 9;
    Radio.Sleep();
    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, currentSF, LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH, LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON, 0, true, 0, 0, false, true);
    Radio.Rx(0); 
    return; 
  } 
  else if (rxPacket == "Cambio SF 12") {
    estadoRx = "¡ALERTA: CAMBIO A SF 12!";
    currentSF = 12;
    Radio.Sleep();
    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, currentSF, LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH, LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON, 0, true, 0, 0, false, true);
    Radio.Rx(0);
    return;
  }
  else if (rxPacket == "Fin") {
    testCompleted = true;
    Radio.Sleep();
    return;
  }

  estadoRx = "¡Dato recibido!";
  Radio.Rx(0); 
}