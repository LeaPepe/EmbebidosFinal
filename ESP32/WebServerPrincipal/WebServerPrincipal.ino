#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <CircularBuffer.h>

// Serial comunication pins
#define RXD2 16
#define TXD2 17

uint16_t count = 0;
// Create Webserver in port 80
WebServer server(80);

// --- SETUP --- // 
void setup(void) {
  
  // Serial comunication
  Serial.begin(115200); // Begin serial (debuging)
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // Begin serial (comunication)
  
  // Initialize File System
  SPIFFSInit();

  // Initialize WiFi
  WiFiInit();

  // Initialize mDNS
  mDNSInit();

  // begin HTTP server
  serverInit();

  serialHello();
  
}

// --- LOOP --- // 
void loop(void) {
  // Handle client (if there is one)
  server.handleClient();
  serialHello();
  delay(1000);
  // Serial Loopback to PC
  while (Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
}

void SPIFFSInit(void) {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}
