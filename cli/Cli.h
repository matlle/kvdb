/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../db/Database.h"
#include "../db/Table.h"

namespace kvdb {

    class Cli {
    public:
        inline static const int8_t OPENDB = 0;
        inline static const int8_t EXIT = 1;
        inline static std::map<std::string, int8_t> cmds{};
        std::string command = std::string();
        std::unique_ptr<Database> db = nullptr;

        Cli();
        void prompt();
        void print_help();
        static int8_t get_command(const std::vector<std::string> &words, const std::string &str_cmd);
        static std::vector<std::string> split_string(const std::string &text, char sep);
    };

} // namespace kvdb

