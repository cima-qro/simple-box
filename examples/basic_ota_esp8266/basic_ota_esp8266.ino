#include <ESP8266WiFi.h>
#include <SimpleBox.h>

// WiFi credentials.
const char* ssid = "YourWifiSSID";
const char* pass = "writeyourpassword";

// Declare a logger and you client
void logger(const char *msg);
SimpleOTA OtaClient("1.1.1.1", 3200, true, logger); // Change the IP, PORT

void setup(){
  Serial.begin(115200);
  
  //Setup WiFi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Debugging purposes
  logger(WiFi.macAddress().c_str());
}

void loop(){
  ++counter

  // Every 250 ms
  if(counter == 125){
    OtaClient.run();
  }

  delay(2); //ms
}

// -------- START OTA --------
void logger(const char *msg){
  Serial.print(msg);
}

// Call when a specific action is triggered
void searchAvailableSketch{
  OtaClient.onDiscoverPressed(param.asInt());
}

// Call when a specific action is triggered
void installSketch{
  OtaClient.onInstallPressed(param.asInt());
}
// -------- END OTA --------