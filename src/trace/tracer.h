#ifndef __CIMA_TRACER_H__
    #define  __CIMA_TRACER_H__
    #include <string>
    #include <vector>
    #include <Arduino.h>

    using namespace std;

    typedef void (*tracer_callback_t) (const char *msg);
    void simple__tracer__out(const char *msg);

    class SimpleTracer {
        private:
            bool debug;
            tracer_callback_t logger;
        public:
            SimpleTracer() : SimpleTracer(false, simple__tracer__out){};
            SimpleTracer(bool debug) : SimpleTracer(debug, simple__tracer__out){};
            SimpleTracer(bool debug, tracer_callback_t logger);
            void println(const vector<string> &args);

    };
#endif