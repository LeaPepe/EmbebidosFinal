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
  server.on("/", handleRoot);

  // Root/inline entry
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  // NotFound entry
  server.onNotFound(handleNotFound);
}




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
