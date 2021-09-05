/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <string>
#include <memory>
#include <pthread.h>
#ifdef OS_WINDOWS
#include <windows.h>
#endif

#define O_READ "rb"
#define O_WRITE "ab+"

#define LOCK() \
if(pthread_mutex_lock(&mutex)) { \
    PRINT_ERROR("%s", strerror(errno)); \
    return 0; \
}

#define UNLOCK() \
if(pthread_mutex_unlock(&mutex)) { \
    PRINT_ERROR("%s", strerror(errno)); \
    return 0; \
}

namespace kvdb {

    class Stream {
    public:
        FILE *file_ptr = nullptr;
        uint64_t total_bytes = 0;
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        std::string path = std::string();
        const char *mode = O_READ;

        Stream(const std::string &path, const char *mode);
        bool seek(const uint32_t &pos) const;
        bool seek_end();
        bool opened() const;
        bool is_write_mode() const;
        uint8_t write_byte(const uint8_t &v);
        uint32_t write_ushort(const uint16_t &v);
        uint32_t write_uint(const uint32_t &v);
        uint32_t write_ulong(const uint64_t &v);
        uint32_t write_string(const std::string &str, bool with_length=true);
        uint8_t read_byte() const;
        uint16_t read_ushort() const;
        uint32_t read_uint() const;
        uint64_t read_ulong() const;
        std::string read_string(uint32_t &len) const;
        bool close();
        bool delete_file() const;
        static bool file_exists(const char *path);
        static bool create_file(const char *path);
    };

}

