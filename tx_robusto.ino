#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>
#include "HT_SSD1306Wire.h"

#define RF_FREQUENCY           927100000 
#define TX_OUTPUT_POWER        14        
#define LORA_BANDWIDTH         0         
#define LORA_CODINGRATE        1         
#define LORA_PREAMBLE_LENGTH   8         
#define LORA_SYMBOL_TIMEOUT    0         
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON   false     

extern SSD1306Wire display;
static RadioEvents_t RadioEvents;

int sequenceNumber = 1;
int currentSF = 7;         
bool testCompleted = false;

// Variables para la Ráfaga de Advertencia
int warningsSent = 0;
const int MAX_WARNINGS = 5; 

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 2000; 
String estadoTx = "Inicializando...";

void setup() {
  Serial.begin(115200);
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(100); 

  display.init();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 0, "Iniciando TX Robusto...");
  display.display();

  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  
  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);
  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    currentSF, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, 3000);
}

void loop() {
  Radio.IrqProcess();
  
  if (millis() - lastSendTime >= sendInterval && !testCompleted) {
    lastSendTime = millis();
    
    // RÁFAGA: Cambio a SF 9
    if (sequenceNumber == 51 && currentSF == 7) {
      if (warningsSent < MAX_WARNINGS) {
        String packet = "Cambio SF 9";
        estadoTx = "Alertando SF9 (" + String(warningsSent+1) + "/5)";
        Radio.Send((uint8_t *)packet.c_str(), packet.length());
        warningsSent++;
      } else {
        // Terminó la ráfaga, procedemos a cambiar la radio del TX
        currentSF = 9;
        Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH, currentSF,
                          LORA_CODINGRATE, LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                          true, 0, 0, LORA_IQ_INVERSION_ON, 3000);
        warningsSent = 0; 
        estadoTx = "Radio TX en SF 9";
      }
    } 
    // RÁFAGA: Cambio a SF 12
    else if (sequenceNumber == 101 && currentSF == 9) {
      if (warningsSent < MAX_WARNINGS) {
        String packet = "Cambio SF 12";
        estadoTx = "Alertando SF12 (" + String(warningsSent+1) + "/5)";
        Radio.Send((uint8_t *)packet.c_str(), packet.length());
        warningsSent++;
      } else {
        currentSF = 12;
        Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH, currentSF,
                          LORA_CODINGRATE, LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                          true, 0, 0, LORA_IQ_INVERSION_ON, 3000);
        warningsSent = 0; 
        estadoTx = "Radio TX en SF 12";
      }
    } 
    // RÁFAGA: Fin de prueba
    else if (sequenceNumber == 151) {
      if (warningsSent < MAX_WARNINGS) {
        String packet = "Fin";
        estadoTx = "Enviando FIN (" + String(warningsSent+1) + "/5)";
        Radio.Send((uint8_t *)packet.c_str(), packet.length());
        warningsSent++;
      } else {
        testCompleted = true;
      }
    } 
    // ENVÍO DE DATOS NORMALES
    else {
      float temp = random(150, 300) / 10.0;
      String packet = "Seq:" + String(sequenceNumber) + " Temp:" + String(temp) + "C";
      estadoTx = "Enviando " + String(sequenceNumber) + "/150";
      Serial.println("Transmitiendo: " + packet);
      Radio.Send((uint8_t *)packet.c_str(), packet.length());
      
      // Solo incrementamos la secuencia si enviamos un dato real
      sequenceNumber++;
    }
  }

  uint16_t bateriaMv = BoardGetBatteryVoltage();
  float bateriaVoltios = bateriaMv / 1000.0;

  display.clear();
  display.drawString(0, 0, "NODO TX Robusto - 927.1");
  if (testCompleted) {
    display.drawString(0, 15, "ESTADO: ENVIO COMPLETADO");
    display.drawString(0, 30, "Total enviado: 150 pqts");
  } else {
    display.drawString(0, 15, "Estado: " + estadoTx);
    display.drawString(0, 30, "SF: " + String(currentSF) + " | BW: 125kHz");
  }
  display.drawString(0, 45, "Bat: " + String(bateriaVoltios, 2) + "V (" + String(bateriaMv) + "mV)");
  display.display();
}

void OnTxDone(void) { Radio.Sleep(); }
void OnTxTimeout(void) { Radio.Sleep(); }