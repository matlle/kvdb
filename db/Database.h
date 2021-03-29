/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <string>
#include <map>
#include "Table.h"

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
        static bool create_dir(const char *dir_name);
    };

} // namespace kvdb
