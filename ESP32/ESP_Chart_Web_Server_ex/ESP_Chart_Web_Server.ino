#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>


// Replace with your network credentials
const char* ssid = "Fibertel WiFi169 2.4GHz";
const char* password = "01416382183";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


float someDataToRead1 = 12.3f;
float someDataToRead2 = 15.3f;

String readData1() {
  float t = someDataToRead2;
  if (isnan(t)) {    
    Serial.println("Failed to read");
    return "";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readData2() {
  float h = someDataToRead1;
  if (isnan(h)) {
    Serial.println("Failed to read");
    return "";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readData1().c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readData2().c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
