#include "tracer.h"

void simple__tracer__out(const char *msg){
    Serial.print(msg);
}

SimpleTracer::SimpleTracer(bool debug, tracer_callback_t logger){
    this -> debug = debug;
    this -> logger = logger;
}

void SimpleTracer::println(const vector<string> &args){
    if(!this -> debug) return;
    for(auto &arg : args){
        this -> logger(arg.c_str());
    }
    this -> logger("\n");
}