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

    struct ThreadWorker {
        bool busy = true;
        Status terminated_status = Status::OK_;

        void set(bool state, Status status) {
            this->busy = state;
            this->terminated_status = status;
        }
    };

    class Database {
    public:
        std::string name = std::string();
        std::string path = std::string();
        bool opened = false;
        std::map<std::string, std::unique_ptr<Table>> tables{};
        std::unique_ptr<ThreadWorker> thread_worker = nullptr;

        explicit Database(const std::string &path);
        bool open();
        void close();
        static bool create_directory(const char *path);
        Table *get_table(const char *table_name);

#ifdef OS_WINDOWS
        static std::string get_last_error_str(DWORD error_id);
#endif
    };

} // namespace kvdb

