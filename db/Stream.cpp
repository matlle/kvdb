/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <sys/stat.h>
#include <cstring>
#include "../utils/definies.h"
#include "../utils/log.hpp"
#include "Stream.h"

namespace kvdb {

    Stream::Stream(const std::string &path, const char *mode) {
        if(path.empty()) {
            ERROR("%s", "stream path empty");
            return;
        }
        struct stat st = {0};
        if(stat(path.c_str(), &st) == -1 || file_ptr == nullptr) {
            file_ptr = fopen(path.c_str(), mode);
            if(file_ptr == nullptr || !seek(0)) {
                return;
            }
        }
        if(!seek_end()) {
            ERROR("%s failed to seek_end", path.c_str());
            return;
        }
        int64_t number_of_bytes = ftell(file_ptr);
        if(number_of_bytes == -1) {
            ERROR("failed ftell %s", strerror(errno));
            return;
        }
        total_bytes = number_of_bytes;
    }

    bool Stream::seek(const uint32_t &pos) const {
        if(fseek(file_ptr, pos, SEEK_SET) != 0) {
            return false;
        }
        return true;
    }

    bool Stream::seek_end() {
        if(fseek(file_ptr, 0, SEEK_END) != 0) {
            fclose(file_ptr);
            file_ptr = nullptr;
            return false;
        }
        return true;
    }

    bool Stream::opened() const {
        return file_ptr != nullptr;
    }

    uint8_t Stream::write_byte(const uint8_t &v) {
        if(file_ptr == nullptr) {
            return 0;
        }
        byte buf[1] = {0};
        buf[0] = v & 0xff;
        LOCK();
        if((fwrite(buf, sizeof(byte), 1, file_ptr) != 1) || fflush(file_ptr) != 0) {
            UNLOCK();
        }
        total_bytes += 1;
        UNLOCK();
        return 1;
    }

    uint32_t Stream::write_uint(const uint32_t &v) {
        if(file_ptr == nullptr) {
            return 0;
        }
        byte buf[4] = {0};
        buf[0] = (v >> 24) & 0xff;
        buf[1] = (v >> 16) & 0xff;
        buf[2] = (v >> 8)  & 0xff;
        buf[3] = v & 0xff;
        LOCK();
        if((fwrite(buf, sizeof(byte), 4, file_ptr) != 4) || fflush(file_ptr) != 0) {
            UNLOCK();
        }
        total_bytes += 4;
        UNLOCK();
        return 4;
    }

    uint32_t Stream::write_ulong(const uint64_t &v) {
        if(file_ptr == nullptr) {
            return 0;
        }
        byte buf[8] = {0};
        buf[0] = (v >> 56) & 0xff;
        buf[1] = (v >> 48) & 0xff;
        buf[2] = (v >> 40) & 0xff;
        buf[3] = (v >> 32) & 0xff;
        buf[4] = (v >> 24) & 0xff;
        buf[5] = (v >> 16) & 0xff;
        buf[6] = (v >> 8)  & 0xff;
        buf[7] = v & 0xff;
        LOCK();
        if((fwrite(buf, sizeof(byte), 8, file_ptr) != 8) || fflush(file_ptr) != 0) {
            UNLOCK();
        }
        total_bytes += 8;
        UNLOCK();
        return 8;
    }

    uint32_t Stream::write_string(const std::string &str, bool with_length) {
        if(with_length && write_uint(str.length()) == 0) {
            return 0;
        }
        LOCK();
        if((fwrite(str.c_str(), sizeof(byte), str.length(), file_ptr) != str.length()) || fflush(file_ptr) != 0) {
            UNLOCK();
        }
        if(with_length) {
            total_bytes += str.length();
        }
        UNLOCK();
        return (with_length ? 4 : 0) + str.length();
    }

    uint8_t Stream::read_byte() const {
        byte buf[1] = {0};
        if(fread(buf, sizeof(byte), 1, file_ptr) != 1) {
            return 0;
        }
        return (uint8_t)buf[0];
    }

    uint32_t Stream::read_uint() const {
        byte buf[4] = {0};
        if(fread(buf, sizeof(byte), 4, file_ptr) != 4) {
            return 0;
        }
        uint32_t d = 0;
        d = (d << 4) + (uint32_t)buf[0];
        d = (d << 4) + (uint32_t)buf[1];
        d = (d << 4) + (uint32_t)buf[2];
        d = (d << 4) + (uint32_t)buf[3];
        return d;
    }

    uint64_t Stream::read_ulong() const {
        byte buf[8] = {0};
        if(fread(buf, sizeof(byte), 8, file_ptr) != 8) {
            return 0;
        }
        uint64_t d = 0;
        d = (d << 8) + (uint64_t)buf[0];
        d = (d << 8) + (uint64_t)buf[1];
        d = (d << 8) + (uint64_t)buf[2];
        d = (d << 8) + (uint64_t)buf[3];
        d = (d << 8) + (uint64_t)buf[4];
        d = (d << 8) + (uint64_t)buf[5];
        d = (d << 8) + (uint64_t)buf[6];
        d = (d << 8) + (uint64_t)buf[7];
        return d;
    }

    std::string Stream::read_string(uint32_t &len) const {
        std::string v = std::string();
        byte *buf = nullptr;
        while((int32_t)len - 255 > 0) {
            len -= 255;
            buf = new (std::nothrow) byte[255]();
            if(fread(buf, sizeof(byte), 255, file_ptr) != 255) {
                v = std::string();
                break;
            }
            std::string t((char *)buf, 255);
            v += t;
        }
        if(len > 0) {
            buf = new (std::nothrow) byte[len]();
            size_t r = fread(buf, sizeof(byte), len, file_ptr);
            if(r == 0) {
                v = std::string();
            } else {
                std::string t((char *)buf, len);
                v += t;
            }
        }
        delete[] buf;
        return v;
    }

}

