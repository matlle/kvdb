/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <sys/stat.h>
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
        uint32_t i = words.size() - 1;
        while(i >= 0 && words.at(i).empty()) {
            i--;
        }
        if(words.at(i).empty()) {
            return false;
        }
        name = words.at(i);
        if(name.empty()) {
            return false;
        }
        if(path.at(path.size() - 1) != '/' && path.at(path.size() - 1) != '\\') {
            path += std::string((const char *)PATH_SEPARATOR);
        }
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
                PRINT_ERROR("%s", get_last_error_str(error_id).c_str());
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

    Table *Database::get_table(const char *table_name) {
        auto it = tables.find(table_name);
        Table *table = nullptr;
        if(it == tables.end()) {
            std::unique_ptr<Table> t = std::make_unique<Table>(table_name, path);
            if(!t->open()) {
                t.reset();
                return nullptr;
            }
            tables.insert(std::pair<std::string, std::unique_ptr<Table>>(table_name, std::move(t)));
            it = tables.find(table_name);
            if(it != tables.end()) {
                table = it->second.get();
            }
        } else {
            table = it->second.get();
        }
        if(table != nullptr) {
            table->db = this;
        }
        return table;
    }

#ifdef OS_WINDOWS
    std::string Database::get_last_error_str(DWORD error_id) {
        if(error_id == 0) {
            return std::string();
        }
        char err_str[256];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err_str, 255, nullptr);
        return std::string((const char *)err_str);
    }
#endif

} // namespace kvdb