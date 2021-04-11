/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "../tree/BTree.h"
#include "Stream.h"

namespace kvdb {

    class Action {
    public:
        static constexpr int8_t PUT = 2;
        static constexpr int8_t GET = 3;
        static constexpr int8_t DELETE = 4;
        std::string table_name;
        int8_t action;
        std::vector <std::vector<std::string>> key_values;

        static int8_t get_action(const std::string &str);
        static std::unique_ptr<Action> parse(const std::string &str);
        static std::string get_key_values_str(const std::vector<std::vector<std::string>> &key_values);
    };

    class Table {
    public:
        std::unique_ptr<btree::BTree> tree = nullptr;
        std::unique_ptr<Stream> stream_data = nullptr;
        std::unique_ptr<Stream> stream_tree = nullptr;
        static uint32_t primary_key;

        explicit Table(const std::string &name, const std::string &db_path);
        kvdb::Status process_action(int8_t action, std::vector<std::vector<std::string>> &key_values, std::vector<std::vector<std::string>> *result);
        std::string get_search_fields(const std::vector<btree::Key *> &keys_found) const;
        void put_keys();
        void get_keys();
        void delete_keys();
    };

} // namespace kvdb