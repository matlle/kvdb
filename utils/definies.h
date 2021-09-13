/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>

#define VERSION "1.0.3"

#if defined(_WIN32) || defined(WIN32)
#define OS_WINDOWS 1
#endif

#ifdef OS_WINDOWS
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/\0"
#endif

#define PAGE_SIZE 5
#define DISK_READ_MAX 1024 * 1024 * 256 // 256 MB
typedef unsigned char byte;

namespace kvdb {
    enum class Status {OK_, ERROR_};

    class StatusEx {
    public:
        static constexpr int8_t ERROR = -1;
        static constexpr int8_t SUCCESS = 0;
        int8_t level = ERROR;
        const char *msg{};

        explicit StatusEx() = default;

        explicit StatusEx(int8_t level, const char *msg) {
            this->level = level;
            this->msg = msg;
        }

        bool is_error() const {
            return level == ERROR;
        }

        bool is_success() const {
            return level == SUCCESS;
        }
    };

    class Error : public StatusEx {
    public:
        explicit Error(const char *msg) : StatusEx(ERROR, msg) {
        }
    };

    class Success : public StatusEx {
    public:
        explicit Success() : StatusEx(SUCCESS, "OK") {
        }

        explicit Success(const char *msg) : StatusEx(SUCCESS, msg) {
        }
    };
}