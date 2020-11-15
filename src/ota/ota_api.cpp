#include "ota_api.h"

SimpleOTA::SimpleOTA(const char *domain, int port, bool debug, tracer_callback_t logger){
    char buffer[10];
    this -> domain = domain;
    this -> new_version = false;
    this -> port = port;

    itoa(this -> port, buffer, 10);
    string host = "http://" + this -> domain + ":" + string(buffer);
    this -> host = host;
    
    this -> tracer = SimpleTracer(debug, logger);
    this -> http_client = SimpleHTTP(host.c_str(), this -> tracer);
    this -> mac_address = WiFi.macAddress().c_str();
}

void SimpleOTA::onDiscoverPressed(int pin){
  if (pin != 1) return;
  this -> discoverUpdate(pin);
}

void SimpleOTA::onInstallPressed(int pin){
  if (pin != 1 || this -> new_version == false) return;
  
  this -> installUpdate(pin);
  this ->new_version = false;
}

void SimpleOTA::run(){
  // Do something
}


void SimpleOTA::discoverUpdate(int pin){
  string response;
  this -> tracer.println({ "[OTA] Searching update..." });
  unsigned char response_code = this -> http_client.get(PATH_DISCOVER, response);
  
  if(response_code != 0) return;

  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  
  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, response);
  
  if (error) {
    this -> tracer.println({ 
      "[OTA] Deserialization process failed: ",
      error.c_str()
    });
    return;
  }

  bool new_version = doc["new_version"].as<bool>();
  if(!new_version){
    this -> new_version = false;
    this -> tracer.println({ "[OTA] No available version" });
    return;
  }

  this -> new_version = true;
  this -> tracer.println({ "[OTA] The available version is" });
  this -> tracer.println({ "[OTA]", doc["id"].as<char*>() });
}

void SimpleOTA::installUpdate(int pin){
  ESPhttpUpdate.update(this -> domain.c_str(), this -> port, PATH_OTA);
}