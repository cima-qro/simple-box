#ifndef __CIMA_OTA_H__
    #define __CIMA_OTA_H__
    #include <ESP8266HTTPClient.h>
    #include <ESP8266httpUpdate.h>
    #include "../http/client.h"
    #include "../trace/tracer.h"
    #define PATH_BEGIN "/iot/begin"
    #define PATH_CONDITION "/iot/condition"
    #define PATH_OTA "/iot/ota"
    #define MAX_OTA_ATTEMPTS 10

    class SimpleOTA {
        private:
            SimpleHTTP http_client;
            SimpleTracer tracer;
            bool new_version;
            bool syncing;
            bool enqueued;
            int port;
            int attempts;
            string domain;
            string host;
            string mac_address;

        public:
            SimpleOTA(const char *domain, int port) : SimpleOTA(domain, port, false, &simple__tracer__out) {};
            SimpleOTA(const char *domain, int port, bool debug) : SimpleOTA(domain, port, debug, &simple__tracer__out) {};
            SimpleOTA(const char *domain, int port, bool debug, tracer_callback_t logger);
            void onDiscoverPressed(int pin);
            void onInstallPressed(int pin);
            void run();
        private:
            void discoverUpdate(int pin);
            void installUpdate(int pin);
            void syncFail(const vector<string> &msgs_per_line);
    };
#endif
