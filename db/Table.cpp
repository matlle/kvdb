/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <sys/stat.h>
#include <cstring>
#include <algorithm>
#include "Table.h"
#include "../cli/Cli.h"
#include "../utils/log.hpp"

namespace kvdb {

    uint32_t Table::primary_key = 1;

    int8_t Action::get_action(const std::string &str) {
        if (str == "put") {
            return PUT;
        }
        if (str == "get") {
            return GET;
        }
        if (str == "delete") {
            return DELETE;
        }
        return -1;
    }

    std::unique_ptr<Action> Action::parse(const std::string &str) {
        std::unique_ptr<Action> action = std::make_unique<Action>();
        std::vector<std::string> words = Cli::split_string(str, '.');
        if(words.empty() || words.size() <= 1) {
            return nullptr;
        }
        action->table_name = words.at(0);
        std::vector<std::string> words1 = Cli::split_string(words.at(1), '(');
        if(words1.empty() || words1.size() <= 1) {
            return nullptr;
        }
        action->op = Action::get_action(words1.at(0));
        std::string action_fields = words1.at(1).substr(0, words1.at(1).size() - 1);
        std::vector<std::string> fields = Cli::split_string(action_fields, ',');
        for(const auto &i : fields) {
            std::vector<std::string> field = Cli::split_string(i, '=');
            if(field.size() > 1 && !field.at(0).empty() && !field.at(1).empty()) {
                action->key_values.push_back(field);
            }
        }
        if(words.size() >= 3) {
            std::string criteria = std::string();
            size_t i = 0;
            while(words.at(2).at(i) != '(') {
                criteria += words.at(2).at(i);
                i++;
            }
            i++;
            if(criteria == "order_by") {
                bool flag = false;
                while(words.at(2).at(i) != ')') {
                    if(words.at(2).at(i) == '=') {
                        flag = true;
                        break;
                    }
                    action->order_by += words.at(2).at(i);
                    i++;
                }
                if(flag) {
                    i++;
                    std::string order = std::string();
                    while(words.at(2).at(i) != ')') {
                        order += words.at(2).at(i);
                        i++;
                    }
                    if(order == "desc") {
                        action->order = order;
                    }
                }
            }
        }
        return action;
    }

    Row::Row(const std::string &row_id) {
        this->row_id = row_id;
    }

    bool Row::get_stream(const std::string &path, const char *mode) {
        stream_data = std::make_unique<Stream>(path + "/r" + row_id, mode);
        if(!stream_data->opened()) {
            if(strcmp(mode, O_AWRITE) == 0) {
                ERROR("%s", "can't open stream_data to awrite");
            }
            return false;
        }
        stream_tree = std::make_unique<Stream>(path + "/" + std::to_string(btree::Node::hash_key("/r" + row_id)), mode);
        if(!stream_tree->opened()) {
            if(strcmp(mode, O_AWRITE) == 0) {
                ERROR("%s", "can't open stream_tree to awrite");
            }
            return false;
        }
        return true;
    }

    std::unordered_map<std::string, std::string> Row::get_data(std::vector<std::vector<std::string>> &key_values, const std::vector<std::string> &fields) const {
        std::unordered_map<std::string, std::string> data{};

        if(!key_values.empty()) {
            std::vector<btree::Key *> keys{};
            if(!has_keys_values(key_values, keys)) {
                return data;
            }
        }

        if(!stream_data->seek(0)) {
            ERROR("%", "failed to seek at start of stream_data");
            return data;
        }

        uint32_t len = 0;
        while((len = stream_data->read_uint()) > 0) {
            std::string key = stream_data->read_string(len);
            std::string value = std::string();
            if(!key.empty() && (len = stream_data->read_uint()) > 0) {
                value = stream_data->read_string(len);
            }
            data[key] = value;
        }
        if(!stream_data->seek_end()) {
            ERROR("%", "failed to seek_end of stream_data");
        }
        return data;
    }

    bool Row::has_value(const btree::Key *key, const std::vector<std::string> &kv) const {
        uint32_t len = 0;
        if(stream_data == nullptr || stream_tree == nullptr) {
            return false;
        }
        if(!stream_data->seek(key->stream_data_pos)) {
            ERROR("%", "failed to seek in stream_data");
            return false;
        }

        if((len = stream_data->read_uint()) <= 0) {
            if(!stream_data->seek_end()) {
                ERROR("%", "failed to seek_end of stream_data");
            }
            return false;
        }

        std::string str_key = stream_data->read_string(len);
        if(str_key.empty() || str_key != kv.at(0)) {
            if(!stream_data->seek_end()) {
                ERROR("%", "failed to seek_end of stream_data");
            }
            return false;
        }

        if((len = stream_data->read_uint()) <= 0) {
            if(!stream_data->seek_end()) {
                ERROR("%", "failed to seek_end of stream_data");
            }
            return false;
        }

        std::string str_value = stream_data->read_string(len);

        if(str_value.empty() || str_value != kv.at(1)) {
            if(!stream_data->seek_end()) {
                ERROR("%", "failed to seek_end of stream_data");
            }
            return false;
        }
        return true;
    }

    bool Row::has_keys_values(const std::vector<std::vector<std::string>> &key_values, std::vector<btree::Key *> keys) const {
        if(key_values.empty()) {
            return false;
        }

        for(const auto &kv: key_values) {
            if(kv.size() <= 1) {
                continue;
            }
            btree::Key *found_key = nullptr;
            std::unique_ptr<btree::Key> key = std::make_unique<btree::Key>(kv.at(0));
            btree::Node *root_node = tree->root;
            btree::Node::search_key(root_node, key.get(), found_key);
            tree->root = btree::BTree::find_root_node(root_node, root_node->parent);
            if(found_key != nullptr) {
                keys.push_back(found_key);
            }
        }

        if(keys.empty() || keys.size() != key_values.size()) {
            return false;
        }

        bool flag = true;
        for(size_t i = 0; i < keys.size(); i++) {
            if(!has_value(keys.at(i), key_values.at(i))) {
                flag = false;
                break;
            }
        }

        if(!flag) {
            return false;
        }

        return true;
    }

    bool Row::delete_row() {
        if(stream_data != nullptr && stream_tree != nullptr) {
            if(stream_data->close() && stream_tree->close()) {
                if(stream_data->delete_file() && stream_tree->delete_file()) {
                    stream_data = nullptr;
                    stream_tree = nullptr;
                    return true;
                }
            }
        }
        return false;
    }

    btree::Key *Row::has_key(const std::string &str_key) const {
        btree::Key *found_key = nullptr;
        std::unique_ptr<btree::Key> key = std::make_unique<btree::Key>(str_key);
        btree::Node *root_node = tree->root;
        btree::Node::search_key(root_node, key.get(), found_key);
        tree->root = btree::BTree::find_root_node(root_node, root_node->parent);
        return found_key;
    }

    void Row::sort_found_rows(std::vector<std::unordered_map<std::string, std::string>> &found_rows, std::unique_ptr<Action> action) {
        // insertion sort
        int i = 1;
        while(i < found_rows.size()) {
            int j = i;
            while(j > 0) {
                if(found_rows.at(j - 1).find(action->order_by) == found_rows.at(j - 1).end()
                   || found_rows.at(j).find(action->order_by) == found_rows.at(j).end()) {
                    std::swap(found_rows.at(j), found_rows.at(j - 1));
                } else {
                    if(action->order == "asc") {
                        if(found_rows.at(j - 1)[action->order_by] > found_rows.at(j)[action->order_by]) {
                            std::swap(found_rows.at(j), found_rows.at(j - 1));
                        }
                    } else if(found_rows.at(j - 1)[action->order_by] < found_rows.at(j)[action->order_by]) {
                        std::swap(found_rows.at(j), found_rows.at(j - 1));
                    }
                }
                j--;
            }
            i++;
        }
    }

    Table::Table(const std::string &name, const std::string &db_path) {
        this->name = name;
        this->path = db_path + "/" + name;
    }

    bool Table::create_dir(const char *dir_name) {
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

    bool Table::open() {
        if(!create_dir(path.c_str())) {
            return false;
        }
        if(stream_info == nullptr) {
            stream_info = std::make_unique<Stream>(path + "/info", O_READONLY);
        }
        return opened = true;
    }

    Row *Table::get_row(const std::string &row_id, bool create_if_not_exists) {
        Row *row = nullptr;
        auto it = rows.find(row_id);
        if(it == rows.end()) {
            std::unique_ptr<Row> row_info = std::make_unique<Row>(row_id);
            if(!row_info->get_stream(path, create_if_not_exists ? O_AWRITE : O_READONLY)) {
                row_info.reset();
                return nullptr;
            }
            if(!row_info->stream_tree->seek(0)) {
                ERROR("%s", "can't seek to start of stream_tree");
                return nullptr;
            }
            row_info->tree = btree::BTree::deserialize(row_info->stream_tree.get());
            if(row_info->tree == nullptr) {
                ERROR("%s", "failed to get tree object");
                return nullptr;
            }
            if(!row_info->stream_tree->seek_end()) {
                ERROR("%s", "can't seek_end stream_tree");
                return nullptr;
            }
            rows.insert(std::pair<std::string, std::unique_ptr<Row>>(row_id, std::move(row_info)));
            it = rows.find(row_id);
            if(it != rows.end()) {
                row = it->second.get();
            }
        } else {
            row = it->second.get();
        }
        return row;
    }

    kvdb::Status Table::process_action(std::unique_ptr<Action> action) {
        kvdb::Status status = kvdb::ERROR;
        if(action->op == Action::PUT) {
            std::string row_id = std::string();
            bool has_not_id_key = true;
            for(const auto &key_value : action->key_values) {
                if(!key_value.empty() && key_value.at(0) == "id") {
                    row_id = key_value.at(1);
                    has_not_id_key = false;
                    break;
                }
            }
            if(has_not_id_key) {
                row_id = std::to_string(primary_key++);
            }

            Row *row = get_row(row_id);
            if(row == nullptr) {
                ERROR("failed to insert row", nullptr);
                return kvdb::ERROR;
            }

            uint32_t bytes_written = 0, stream_data_pos = 0;
            for(const auto &kv : action->key_values) {
                if(kv.size() <= 1) {
                    continue;
                }
                kvdb::btree::Key *found_key = row->has_key(kv.at(0));
                if(found_key != nullptr) {
                    if(kv.at(0) == "id") {
                        continue;
                    }
                    found_key->deleted = true;
                    btree::Node *root_node = row->tree->root;
                    row->tree->root = btree::Node::delete_key(root_node, found_key, nullptr);
                }

                bytes_written = row->stream_data->write_string(kv.at(0));
                bytes_written += row->stream_data->write_string(kv.at(1));
                stream_data_pos = row->stream_data->total_bytes - bytes_written;
                std::unique_ptr<btree::Key> key = std::make_unique<btree::Key>(kv.at(0));
                key->stream_data_pos = stream_data_pos;
                if(key->serialize(row->stream_tree.get())) {
                    row->tree->root = row->tree->root->insert_key_to_leaf(std::move(key));
                }
            }

            status = kvdb::OK;
        } else if(action->op == Action::GET) {
            std::vector<std::unordered_map<std::string, std::string>> found_rows{};
            std::vector<std::string> fields{};

            if(rows.empty()) {
                int i = 1;
                while(get_row(std::to_string(i), false) != nullptr) {
                    i++;
                }
            }

            std::map<std::string, std::unique_ptr<Row>>::iterator it;
            for(it = rows.begin(); it != rows.end(); it++) {
                std::unordered_map<std::string, std::string> data = it->second->get_data(action->key_values, fields);
                if(!data.empty()) {
                    found_rows.push_back(data);
                }
            }

            if(found_rows.empty()) {
                PRINT("No rows found", nullptr);
            } else {
                if(!action->order_by.empty()) {
                    Row::sort_found_rows(found_rows, std::move(action));
                }
                PRINT("(%u) row%s found", found_rows.size(), (found_rows.size() > 1 ? "s" : ""));
                display_found_rows(found_rows);
            }
            status = kvdb::OK;
        } else if(action->op == Action::DELETE) {
            if(rows.empty()) {
                int i = 1;
                while(get_row(std::to_string(i), false) != nullptr) {
                    i++;
                }
            }
            uint32_t deleted_rows_count = 0;
            std::vector<btree::Key *> keys{};
            for(auto it = rows.cbegin(), next_it = it; it != rows.cend(); it = next_it) {
                ++next_it;
                if(!action->key_values.empty()) {
                    if(it->second->has_keys_values(action->key_values, keys) && it->second->delete_row()) {
                        deleted_rows_count++;
                        rows.erase(it);
                    }
                } else if(it->second->delete_row()) {
                    deleted_rows_count++;
                    rows.erase(it);
                }
            }

            if(deleted_rows_count == 0) {
                PRINT("%s", "No rows deleted");
            } else {
                PRINT("%u row%s deleted", deleted_rows_count, (deleted_rows_count > 1 ? "s" : ""));
            }
            status = kvdb::OK;
        }
        return status;
    }

    void Table::display_found_rows(const std::vector<std::unordered_map<std::string, std::string>> &found_rows) {
        if(found_rows.empty()) {
            return;
        }
        std::string str = std::string();
        std::string row = std::string();
        for(auto found_row : found_rows) {
            if(found_row.empty()) {
                continue;
            }

            std::unordered_map<std::string, std::string>::iterator it;
            size_t j = 0;
            std::string s = std::string();
            for(it = found_row.begin(); it != found_row.end(); ++it) {
                s = it->first + ": " + it->second;
                row.insert(0, s);
                if(j + 1 < found_row.size()) {
                    row.insert(0, ", ");
                }
                j++;
            }

            row += "}";
            row.insert(0, "{");

            if(row.length() > 2) {
                if(!str.empty()) {
                    str += "\n";
                }
                str += row;
                row = "";
            }
        }
        PRINT("%s", str.c_str());
    }

} // namespace kvdb