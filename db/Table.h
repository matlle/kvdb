/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <string>
#include <map>
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

    struct KeyValue {
        std::string key = std::string();
        std::string value = std::string();
    };

    class Row {
    public:
        std::string row_id;
        std::unique_ptr<Stream> stream_tree = nullptr;
        std::unique_ptr<Stream> stream_data = nullptr;
        std::unique_ptr<btree::BTree> tree = nullptr;

        explicit Row(const std::string &row_id);

        bool get_stream(const std::string &path);
        std::vector<KeyValue> get_data(std::vector<std::vector<std::string>> &key_values, const std::vector<std::string> &fields) const;
        bool has_value(const btree::Key *key, const std::vector<std::string> &kv) const;
    };

    class Table {
    public:
        static uint32_t primary_key;
        std::string name = std::string();
        std::string path = std::string();
        bool opened = false;
        std::map<std::string, std::unique_ptr<Row>> rows{};

        explicit Table(const std::string &name, const std::string &db_path);
        kvdb::Status process_action(int8_t action, std::vector<std::vector<std::string>> &key_values);
        static void display_found_rows(const std::vector<std::vector<KeyValue>> &found_rows);
        bool open();
        static bool create_dir(const char *dir_name);
        Row *get_row_info(const std::string &row_id);
    };

} // namespace kvdb