/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <string>
#include <map>
#include "Table.h"
#ifdef OS_WINDOWS
#include <windows.h>
#endif

namespace kvdb {

    class Database {
    public:
        std::string name = std::string();
        std::string path = std::string();
        bool opened = false;
        std::map<std::string, std::unique_ptr<Table>> tables{};

        explicit Database(const std::string &path);
        bool open();
        void close();
        static bool create_directory(const char *path);
        Table *get_table(const char *table_name);

#ifdef OS_WINDOWS
        static std::string get_last_error_msg(DWORD error_id);
#endif
    };

} // namespace kvdb

