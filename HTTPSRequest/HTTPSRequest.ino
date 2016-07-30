// Library
#include <ESP8266WiFi.h>
#include "DHT.h"

// DHT sensor settings
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

// Baud 
const int baud = 115200;

// WiFi 
const char* ssid = "deny-2.4G";
const char* password = "960902463";

// Time to sleep (in seconds):
const int sleepTimeS = 10;

// Host
const char* host = "10.0.0.120";
const int httpPort = 8234;
const char* url = "/upload";

String getSensorsJson(){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h)) {
        h = 0.00;
    }
    if (isnan(t)) {
        t = 0.00;
    }

    String res = "{\"temperature\": ";
    res += (String)t;
    res += ",\"humidity\": ";
    res += (String)h;
    res += "}";

    return res;
}

void setup() 
{
  // Serial
  Serial.begin(baud);
  Serial.println("ESP8266 in normal mode");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Print the IP address
  Serial.println(WiFi.localIP());  

  // DHT
  dht.begin();
}

void loop() 
{
  // Logging data to cloud
  Serial.print("Connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // This will send the request to the server
  client.print(String("GET ")+String(url) + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               getSensorsJson() +
               "\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");

  // Sleep
  Serial.println("ESP8266 in sleep mode");
  ESP.deepSleep(sleepTimeS * 1000000);
  delay(sleepTimeS * 1000);
}
