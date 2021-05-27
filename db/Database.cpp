/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <cstring>
#include <vector>
#include "Database.h"
#include "../utils/log.hpp"
#include "../cli/Cli.h"


namespace kvdb {

    Database::Database(const std::string &path) {
        this->path = path;
    }

    bool Database::open() {
        if(!create_directory(path.c_str())) {
            PRINT_ERROR("failed to create database directory", nullptr);
            return false;
        }
#ifdef OS_WINDOWS
        std::vector<std::string> words = Cli::split_string(path, '\\');
#else
        std::vector<std::string> words = Cli::split_string(path, '/');
#endif
        if(words.empty()) {
            return false;
        }
        name = words.at(words.size() - 1);
        if(name.empty()) {
            return false;
        }
        path += std::string((const char *)PATH_SEPARATOR);
        return opened = true;
    }

    void Database::close() {
        opened = false;
    }

    bool Database::create_directory(const char *path) {
#ifdef OS_WINDOWS
        if(!CreateDirectory(path, nullptr)) {
            DWORD error_id = GetLastError();
            if(error_id != ERROR_ALREADY_EXISTS) {
                PRINT_ERROR("%s", get_last_error_msg(error_id).c_str());
                return false;
            }
        }
#endif
#ifndef OS_WINDOWS
        struct stat st = {0};
        if(stat(path, &st) == -1) {
            int r = mkdir(path, 0777);
            if(r != 0 && r != EEXIST)  {
                PRINT_ERROR("failed to create directory: %s", strerror(errno));
                return false;
            }
        }
#endif
        return true;
    }

#ifdef OS_WINDOWS
    std::string Database::get_last_error_msg(DWORD error_id) {
        if(error_id == 0) {
            return std::string();
        }
        //return std::system_category().message(error_id);
        return std::string();
    }
#endif

} // namespace kvdb