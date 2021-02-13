
// Network parameters
const char* ssid = "Gimenez WiFi 2.4GHz";
const char* password = "leandronahuel";
const char* host = "phasor";


// WiFi connection Initialization
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


// mDNS page name initialization
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
