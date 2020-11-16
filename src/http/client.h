#ifndef __SIMPLE_HTTP_H__
#define __SIMPLE_HTTP_H__
#include <string>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "../vendor/ArduinoJson.h"
#include "../trace/tracer.h"
#include "../general/global.h"

  using namespace std;

  class SimpleHTTP{
    private:
      SimpleTracer tracer;
      WiFiClient wifi_client;
      string host;
    
    public:
      SimpleHTTP();
      SimpleHTTP(const char *host, SimpleTracer &tracer);
      unsigned char get(const char *path, string &response);
  };
#endif
