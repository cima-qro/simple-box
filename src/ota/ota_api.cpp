#include "ota_api.h"

SimpleOTA::SimpleOTA(const char *domain, int port, bool debug, tracer_callback_t logger){
    char buffer[10];
    this -> domain = domain;
    this -> new_version = false;
    this -> syncing = false;
    this -> enqueued = false;
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
  // Recommend to avoid multiple requests
  if(this -> syncing) return;

  this -> syncing = true;
  this -> new_version = false;
  this -> enqueued = false;
  this -> discoverUpdate(pin);
}

void SimpleOTA::onInstallPressed(int pin){
  if (pin != 1) return;
  // Only if a sketch is available to be downloaded
  if (!this -> new_version) return;
  // Recommend to reduce the errors of sync
  if(this -> syncing) return;

  // Main behaviour
  this -> installUpdate(pin);
}

void SimpleOTA::run(){
  // Do something
}


void SimpleOTA::discoverUpdate(int pin){
  string response;
  this -> tracer.println({ "[OTA] Searching update..." });
  unsigned char response_code = this -> http_client.get(PATH_BEGIN, response);
  
  if(response_code != 0) return;

  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  
  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, response);
  
  if (error) {
    this -> syncFail({
      "Deserialization process failed",
      error.c_str()
    });
    return;
  }

  bool enqueued = doc["enqueued"].as<bool>();
  if(!enqueued){
    this -> syncFail({
      "Could not enqueue the sketch process"
    });
    return;
  }

  this -> enqueued = true;
  this -> tracer.println({ "[OTA] Enqueued correctly" });
}

void SimpleOTA::installUpdate(int pin){
  ESPhttpUpdate.update(this -> domain.c_str(), this -> port, PATH_OTA);
}

void SimpleOTA::syncFail(const vector<string> &msgs_per_line){
  this -> syncing = false;
  this -> new_version = false;

  size_t lines = msgs_per_line.size();
  for(size_t i = 0; i < lines; ++i){
    this -> tracer.println({ 
    "[OTA][ERROR]",
    msgs_per_line[i]
  });
  }
}