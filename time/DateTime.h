/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <string>
#include <ctime>

class DateTime {
public:
    int sec = 0;
    int min = 0;
    int hour = 0;
    int day = 0;
    int month = 0;
    int year = 0;

    DateTime();
    static long now_timestamp_seconds();
    static std::string datetime();
};