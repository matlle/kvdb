/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <sys/stat.h>
#include <vector>
#include "Database.h"
#include "../utils/log.hpp"
#include "../cli/Cli.h"

namespace kvdb {

    Database::Database(const std::string &path) {
        this->path = path;
    }

    bool Database::open() {
        if(!create_dir(path.c_str())) {
            return false;
        }
        std::vector<std::string> words = Cli::split_string(path, '/');
        if(words.empty()) {
            return false;
        }
        name = words.at(words.size() - 1);
        if(name.empty()) {
            return false;
        }
        return opened = true;
    }

    void Database::close() {
        opened = false;
    }

    bool Database::create_dir(const char *dir_name) {
        struct stat st = {0};
        if(stat(dir_name, &st) == -1) {
#ifdef __linux__
            int r = mkdir(dir_name, 0777);
            if(r != 0 && r != EEXIST)  {
                ERROR("%s", "mkdir() failed");
                return false;
            }
#else
            _mkdir(dir_name);
#endif

        }
        return true;
    }

} // namespace kvdb