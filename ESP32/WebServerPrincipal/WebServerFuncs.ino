// HTTP server initialization
void serverInit()
{
  server.begin();
  Serial.println("HTTP server started");
  
}

// Hook all entries in the http page
void hookEntries()
{
  // Root entry
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });

  server.on("/params", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", stringParams.c_str());
  });

  server.on("/samples", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", stringSamples.c_str());
  });

  // NotFound entry
  server.onNotFound(handleNotFound);
}

// on 404 not found page
void handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "404 - Not found");
}
