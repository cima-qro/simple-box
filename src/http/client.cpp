#include "client.h"

SimpleHTTP::SimpleHTTP(){
  this -> host = "nohost";
  this -> tracer = SimpleTracer(false);
}

SimpleHTTP::SimpleHTTP(const char *host, SimpleTracer &tracer){
  this -> host = host;
  this -> tracer = tracer;
}

unsigned char SimpleHTTP::get(const char *path, string &response){
  HTTPClient http_client;
  string url = this -> host + path;

  if(!http_client.begin(this->wifi_client, url.c_str())){
    this -> tracer.println({ "[HTTP] Unable to connect" });
    return 1;
  }


  http_client.addHeader(F("x-ESP8266-STA-MAC"), WiFi.macAddress());
  http_client.addHeader(F("x-ESP8266-sketch-md5"), String(ESP.getSketchMD5()));
  http_client.addHeader(F("x-ESP8266-sdk-version"), ESP.getSdkVersion());


  this -> tracer.println({ "[HTTP] GET..." });
  int http_code = http_client.GET();

  if (http_code <= 0) {
    this -> tracer.println({
      "[HTTP] GET... failed, error: ",
      http_client.errorToString(http_code).c_str()
    });
    return 2;
  }
  
  this -> tracer.println({ "[HTTP] GET... code: ", i2str(http_code) });

  if (http_code == HTTP_CODE_OK || http_code == HTTP_CODE_MOVED_PERMANENTLY) {
    response = http_client.getString().c_str();
  }  

  http_client.end();
  return 0;
}