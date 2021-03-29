/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include "../time/DateTime.h"

#define PRINT(str, args...) \
out(0, str, args)

#define ERROR(str, args...) \
out(1, str, args)

template<typename... Args>
void out(uint8_t type, const char *str, Args... args) {
    std::string text = std::string((const char *)str);
    if(type == 1) {
        text = "[ERROR] " + text;
        //text = "[ERROR] " + std::string((const char *)DateTime::datetime().c_str()) + " - " + text;
    }
    text += "\n";
    printf(text.c_str(), args...);
}