#include "global.h"

std::string i2str(const int &number){
    char buffer[10];
    snprintf (buffer, sizeof(buffer), "%d", number);
    return std::string(buffer);
}