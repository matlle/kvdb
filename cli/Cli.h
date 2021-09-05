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
        static constexpr int8_t SELECTDB = 0;
        static constexpr int8_t EXIT = 1;
        static constexpr int8_t HELP = 2;
        static constexpr int8_t QUERYLOAD = 3;
        std::string command = std::string();
        std::unique_ptr<Database> db = nullptr;

        Cli();
        void prompt();
        void print_help();
        static int8_t get_command(const std::vector<std::string> &words, const std::string &str_cmd);
        static std::vector<std::string> split_string(const std::string &text, char sep);
        static bool is_table_op(int8_t cmd);
    };

} // namespace kvdb

