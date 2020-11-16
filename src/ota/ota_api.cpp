#include "ota_api.h"

SimpleOTA::SimpleOTA(const char *domain, int port, bool debug, tracer_callback_t logger){
    this -> domain = domain;
    this -> new_version = false;
    this -> syncing = false;
    this -> enqueued = false;
    this -> version_printed = false;
    this -> attempts = 0;
    this -> port = port;

    string host = "http://" + this -> domain + ":" + i2str(this -> port);
    this -> tracer = SimpleTracer(debug, logger);
    this -> http_client = SimpleHTTP(host.c_str(), this -> tracer);
}

void SimpleOTA::onDiscoverPressed(int pin){
  if (pin != 1) return;
  // Recommend to avoid multiple requests
  if(this -> syncing) return;

  this -> syncing = true;
  this -> new_version = false; // Force a reset
  this -> enqueued = false;
  this -> attempts = 0;
  this -> discoverUpdate(pin);
}

void SimpleOTA::onInstallPressed(int pin){
  if (pin != 1) return;
  // Recommend to reduce the errors of sync
  if(this -> syncing) return;
  // Only if a sketch is available to be downloaded
  if (!this -> new_version){
    this -> tracer.println({ "[OTA] No version available" });
    return;
  }

  // Main behaviour
  this -> installUpdate(pin);
}

void SimpleOTA::run(){
  if(this -> version_printed == false){
    this -> version_printed = true;
    this -> tracer.println({ SIMPLE_BOX_VERSION });
  }

  if(! this -> enqueued) return;
  const int attempts = this -> attempts + 1;
  this -> attempts = attempts;

  string response;
  this -> tracer.println({ "[OTA] Attempt " + i2str(attempts) + "..." });

  if(attempts > 10){
    this -> syncFail({
      "Max attempts overflow"
    });
    return;
  }

  unsigned char response_code = this -> http_client.get(PATH_CONDITION, response);

  if(response_code != 0) {
    if(attempts == MAX_OTA_ATTEMPTS){
      this -> syncFail({
        "HTTP error"
      });
    }
    return;
  }

  // https://arduinojson.org/v6/assistant/
  const size_t capacity = JSON_OBJECT_SIZE(2) + 30;
  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, response);

  if (error) {
    if(attempts == MAX_OTA_ATTEMPTS){
      this -> syncFail({
        "Deserialization process failed",
        error.c_str()
      });
    }
    return;
  }

  bool syncing = doc["syncing"].as<bool>();

  if(syncing){
    if(attempts == MAX_OTA_ATTEMPTS){
      this -> syncFail({
        "Max attempts reached and sync is still"
      });
    }
    return;
  }

  bool downloaded = doc["downloaded"].as<bool>();

  if(!downloaded){
    this -> syncFail({
      "Could not found a sketch to download",
      error.c_str()
    });
    return;
  }

  this -> tracer.println({ "[OTA] Sketch found!!" });
  this -> new_version = true;
  this -> syncing = false;
  this -> enqueued = false;
  this -> attempts = 0;
}


void SimpleOTA::discoverUpdate(int pin){
  string response;
  this -> tracer.println({ "[OTA] Searching update..." });
  unsigned char response_code = this -> http_client.get(PATH_BEGIN, response);
  
  if(response_code != 0) {
    this -> syncFail({
      "HTTP error"
    });
    return;
  }

  // https://arduinojson.org/v6/assistant/
  const size_t capacity = JSON_OBJECT_SIZE(1) + 10;
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
  this -> enqueued = false;
  this -> attempts = 0;

  size_t lines = msgs_per_line.size();
  for(size_t i = 0; i < lines; ++i){
    this -> tracer.println({ 
    "[OTA][ERROR]",
    msgs_per_line[i]
  });
  }
}