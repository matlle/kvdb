/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "DateTime.h"

DateTime::DateTime() {
    time_t seconds = time(nullptr);
    struct tm *time_info = localtime(&seconds);;
    sec = time_info->tm_sec;
    min = time_info->tm_min;
    hour = time_info->tm_hour;
    day = time_info->tm_mday;
    month = time_info->tm_mon + 1;
    year = time_info->tm_year + 1900;
}

std::string DateTime::datetime() {
    DateTime dt;
    char buf[30];
    sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d", dt.day, dt.month, dt.year, dt.hour, dt.min, dt.sec);
    std::string str(buf);
    return str;
}

long DateTime::now_timestamp_seconds() {
    return time(nullptr);
}

std::string DateTime::duration(double ms) {
    std::string str = std::string();
    char buf[20] = {0};
    sprintf(buf, "%.4fs", ms / 1000);
    /*if((ms / 1000.0) < 1) {
        sprintf(buf, "%.4fms", ms);
    } else if((ms / 1000.0) > 1) {
        sprintf(buf, "%.4fs", ms);
    } else if((ms / 60000.0) > 1) {
        sprintf(buf, "%.4fmn", ms);
    }*/
    return std::string((const char *)buf);
}
