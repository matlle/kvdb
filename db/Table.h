/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "../tree/BTree.h"

namespace kvdb {

    class Action {
    public:
        inline static const int8_t PUT = 2;
        inline static const int8_t GET = 3;
        inline static const int8_t DELETE = 4;
        std::string table_name;
        int8_t action;
        std::vector <std::vector<std::string>> fields;

        static int8_t get_action(const std::string &str);
        static std::unique_ptr<Action> parse(const std::string &str);

    };

    class Table {
    public:
        std::unique_ptr<btree::BTree> tree = nullptr;
    };

} // namespace kvdb