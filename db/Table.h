/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include "../tree/BTree.h"
#include "Stream.h"

namespace kvdb {

    class Action {
    public:
        static constexpr int8_t PUT = 3;
        static constexpr int8_t GET = 4;
        static constexpr int8_t DELETE = 5;
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

        bool get_stream(const std::string &path, const char *mode=O_AWRITE);
        //std::vector<KeyValue> get_data(std::vector<std::vector<std::string>> &key_values, const std::vector<std::string> &fields) const;
        std::unordered_map<std::string, std::string> get_data(std::vector<std::vector<std::string>> &key_values, const std::vector<std::string> &fields) const;
        btree::Key *has_key(const std::string &str_key) const;
        bool has_value(const btree::Key *key, const std::vector<std::string> &kv) const;
        bool has_keys_values(const std::vector<std::vector<std::string>> &key_values, std::vector<btree::Key *> keys) const;
        bool delete_row();
    };

    class Table {
    public:
        static uint32_t primary_key;
        std::string name = std::string();
        std::string path = std::string();
        bool opened = false;
        std::map<std::string, std::unique_ptr<Row>> rows{};
        std::unique_ptr<Stream> stream_info = nullptr;

        explicit Table(const std::string &name, const std::string &db_path);
        kvdb::Status process_action(int8_t action, std::vector<std::vector<std::string>> &key_values);
        //static void display_found_rows(const std::vector<std::vector<KeyValue>> &found_rows);
        static void display_found_rows(const std::vector<std::unordered_map<std::string, std::string>> &found_rows);
        bool open();
        static bool create_dir(const char *dir_name);
        Row *get_row(const std::string &row_id, bool create_if_not_exists=true);
    };

} // namespace kvdb