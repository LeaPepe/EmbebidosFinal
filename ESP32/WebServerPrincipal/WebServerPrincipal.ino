#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <CircularBuffer.h>
#include <ArduinoJson.h>

// Serial comunication pins
#define RXD2 16
#define TXD2 17

// JSON Data to WebServer
StaticJsonDocument<300> docParams; // JSON to send data to http page
StaticJsonDocument<2000> docSamples; // JSON to send data to http page
String stringParams, stringSamples;

// Time and count used to request
unsigned long timeStart;
uint16_t count = 0;

// Create Webserver in port 80
AsyncWebServer server(80);

// --- SETUP --- // 
void setup(void) {
  // Serial comunication
  Serial.begin(115200); // Begin serial (debuging)
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // Begin serial (comunication)
  
  initSamples();
  
  // Initialize File System
  SPIFFSInit();
  // Initialize WiFi
  WiFiInit();
  // Initialize mDNS
  mDNSInit();

  hookEntries();

  
  // begin HTTP server
  serverInit();

  
  serialHello();
  timeStart = millis();
}

// --- LOOP --- // 
void loop(void) {
  
  // 10 seconds
  if(millis() - timeStart >= 10000){
    requestSamples();
    timeStart = millis();
  }
  
  handleUart();
}

void SPIFFSInit(void) {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}
