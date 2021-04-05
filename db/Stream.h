/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <string>
#include <memory>
#include <pthread.h>

#define O_READONLY "rb"
#define O_AWRITE "ab+"

#define LOCK() \
if(pthread_mutex_lock(&mutex)) { \
    ERROR("%s", strerror(errno)); \
    return 0; \
}

#define UNLOCK() \
if(pthread_mutex_unlock(&mutex)) { \
    ERROR("%s", strerror(errno)); \
    return 0; \
}

namespace kvdb {

    class Stream {
    public:
        FILE *file_ptr = nullptr;
        uint64_t total_bytes = 0;
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

        Stream(const std::string &path, const char *mode);
        [[nodiscard]] bool seek(const uint32_t &pos) const;
        [[nodiscard]] bool seek_end();
        [[nodiscard]] bool opened() const;
        [[nodiscard]] uint8_t write_byte(const uint8_t &v);
        [[nodiscard]] uint32_t write_uint(const uint32_t &v);
        [[nodiscard]] uint32_t write_ulong(const uint64_t &v);
        [[nodiscard]] uint32_t write_string(const std::string &str);
        [[nodiscard]] uint8_t read_byte() const;
        [[nodiscard]] uint32_t read_uint() const;
        [[nodiscard]] uint64_t read_ulong() const;
        [[nodiscard]] std::string read_string(uint32_t &len) const;
    };

}

