#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>

#define RXD2 16
#define TXD2 17

// Network parameters
const char* ssid = "Fibertel WiFi169 2.4GHz";
const char* password = "01416382183";
const char* host = "threephases";

// Create Webserver in port 80
WebServer server(80);

// On root callback
void handleRoot() {
  // send page as 200 SUCCESS
  server.send(200, "text/plain", "hello from esp8266!");
}

// on 404 not found page
void handleNotFound() {
  // Create the not found message
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  // Send message as 404 Page Not Found
  server.send(404, "text/plain", message);
}

void SPIFFSInit(void) {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

void mDNSInit(void)
{
  if (MDNS.begin(host)) {
    Serial.println("MDNS started!");
    Serial.print("Open http://");
    Serial.print(host);
    Serial.println(".local/");
  } else {
    Serial.println("Couldn't start MDNS!");
  }
}

void WiFiInit(void)
{
  // Connect to network
  WiFi.mode(WIFI_STA);

  // Begin WiFi session
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection session to begin
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup(void) {

  // Begin serial (debuging)
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  // Initialize SPIFFS

  SPIFFSInit();

  // Initialize WiFi
  WiFiInit();

  // Initialize mDNS
  mDNSInit();

  // On / entry
  server.on("/", handleRoot);

  // on /inline, send a plain text
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  // on server not found, execute the function handleNotFound
  server.onNotFound(handleNotFound);

  // begin HTTP server
  server.begin();
  Serial.println("HTTP server started");
  Serial2.println("ESP32 Connected!");
}

void loop(void) {
  // Handle client (if there is one)
  server.handleClient();

  // Serial Loopback
  while (Serial2.available()) {
    Serial.print(char(Serial2.read()));
  }
}


float someDataToRead1 = 220.0f;
String readData1() {
  float t = someDataToRead1;
  if (isnan(t)) {
    Serial.println("Failed to read");
    return "";
  }
  else {
    //Serial.println(t);
    return String(t);
  }
}

float someDataToRead2 = 10.0f;
String readData2() {
  float h = someDataToRead2;
  if (isnan(h)) {
    Serial.println("Failed to read");
    return "";
  }
  else {
    //Serial.println(h);
    return String(h);
  }
}
