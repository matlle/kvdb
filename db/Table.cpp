/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <sys/stat.h>
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
        words = Cli::split_string(words.at(1), '(');
        if(words.empty() || words.size() <= 1) {
            return nullptr;
        }
        action->action = Action::get_action(words.at(0));
        std::string action_fields = words.at(1).substr(0, words.at(1).size() - 1);
        std::vector<std::string> fields = Cli::split_string(action_fields, ',');
        for(const auto &i : fields) {
            std::vector<std::string> field = Cli::split_string(i, '=');
            action->key_values.push_back(field);
        }
        return action;
    }

    std::string Action::get_key_values_str(const std::vector<std::vector<std::string>> &key_values) {
        std::string str = std::string();
        for(size_t i = 0; i < key_values.size(); i++) {
            if(key_values.at(i).size() <= 1) {
                continue;
            }
            str += key_values.at(i).at(0);
            str += "=";
            str += key_values.at(i).at(1);
            if(i + 1 < key_values.size()) {
                str += "&";
            }
        }
        return str;
    }

    Row::Row(const std::string &row_id) {
        this->row_id = row_id;
    }

    bool Row::get_stream(const std::string &path) {
        stream_data = std::make_unique<Stream>(path + "/r" + row_id, O_AWRITE);
        if(!stream_data->opened()) {
            ERROR("%s", "can't open stream");
            return false;
        }
        stream_tree = std::make_unique<Stream>(path + "/r" + row_id + "_" + std::to_string(btree::Node::hash_key("/r" + row_id)), O_AWRITE);
        if(!stream_tree->opened()) {
            ERROR("%s", "can't open stream");
            return false;
        }
        return true;
    }

    std::vector<KeyValue> Row::get_data(std::vector<std::vector<std::string>> &key_values, const std::vector<std::string> &fields) const {
        std::vector<KeyValue> data{};
        if(key_values.empty()) {
            return data;
        }

        std::vector<btree::Key *> keys{};
        for(const auto &kv: key_values) {
            if(kv.size() <= 1) {
                continue;
            }
            btree::Key *found_key = nullptr;
            std::unique_ptr<btree::Key> key = std::make_unique<kvdb::btree::Key>(kv.at(0));
            btree::Node *root_node = tree->root;
            btree::Node::search_key(root_node, key.get(), found_key);
            tree->root = btree::BTree::find_root_node(root_node, root_node->parent);
            if(found_key != nullptr) {
                keys.push_back(found_key);
            }
        }

        if(keys.empty() || keys.size() != key_values.size()) {
            return data;
        }

        std::string str_key = std::string();
        std::string str_value = std::string();
        bool flag = true;
        for(size_t i = 0; i < keys.size(); i++) {
            if(!has_value(keys.at(i), key_values.at(i))) {
                flag = false;
                break;
            }
        }

        if(!flag) {
            return data;
        }

        if(!stream_data->seek(0)) {
            ERROR("%", "failed to seek at start of stream_data");
        }

        uint32_t len = 0;
        while((len = stream_data->read_uint()) > 0) {
            KeyValue keyValue;
            keyValue.key = stream_data->read_string(len);
            if(!keyValue.key.empty() && (len = stream_data->read_uint()) > 0) {
                keyValue.value = stream_data->read_string(len);
            }
            data.push_back(keyValue);
        }
        if(!stream_data->seek_end()) {
            ERROR("%", "failed to seek_end of stream_data");
        }
        return data;
    }

    bool Row::has_value(const btree::Key *key, const std::vector<std::string> &kv) const {
        uint32_t len = 0;
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
        return opened = true;
    }

    Row *Table::get_row_info(const std::string &row_id) {
        Row *row = nullptr;
        auto it = rows.find(row_id);
        if(it == rows.end()) {
            std::unique_ptr<Row> row_info = std::make_unique<Row>(row_id);
            if(!row_info->get_stream(path)) {
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
        }
        return row;
    }

    kvdb::Status Table::process_action(int8_t action, std::vector<std::vector<std::string>> &key_values) {
        kvdb::Status status = kvdb::ERROR;
        if(action == Action::PUT) {
            std::string row_id = std::string();
            bool has_not_id_key = true;
            for(const auto &key_value : key_values) {
                if(!key_value.empty() && key_value.at(0) == "id") {
                    row_id = key_value.at(1);
                    has_not_id_key = false;
                    break;
                }
            }
            if(has_not_id_key) {
                row_id = std::to_string(primary_key++);
            }

            Row *row_info = get_row_info(row_id);
            if(row_info == nullptr) {
                ERROR("%s", "failed to insert row");
                return kvdb::ERROR;
            }

            for(const auto &kv : key_values) {
                if(kv.size() <= 1) {
                    continue;
                }
                uint32_t bytes_written = row_info->stream_data->write_string(kv.at(0));
                bytes_written += row_info->stream_data->write_string(kv.at(1));
                uint32_t stream_data_pos = row_info->stream_data->total_bytes - bytes_written;
                std::unique_ptr<kvdb::btree::Key> key = std::make_unique<kvdb::btree::Key>(kv.at(0));
                key->stream_data_pos = stream_data_pos;
                if(key->serialize(row_info->stream_tree.get())) {
                    row_info->tree->root = row_info->tree->root->insert_key_to_leaf(std::move(key));
                }
            }
            status = kvdb::OK;
        } else if(action == Action::GET) {
            std::vector<std::vector<KeyValue>> found_rows{};
            std::vector<std::string> fields{};

            std::map<std::string, std::unique_ptr<Row>>::iterator it;
            for(it = rows.begin(); it != rows.end(); it++) {
                std::vector<KeyValue> data = it->second->get_data(key_values, fields);
                if(!data.empty()) {
                    found_rows.push_back(data);
                }
            }

            if(found_rows.empty()) {
                PRINT("%s", "No rows found");
            } else {
                PRINT("(%u) row%s found", found_rows.size(), (found_rows.size() > 1 ? "s" : ""));
                if(!found_rows.empty()) {
                    display_found_rows(found_rows);
                }
            }
            status = kvdb::OK;
        } else if(action == Action::DELETE) {
            /*std::vector<btree::Key *> keys_found{};
            uint32_t count_keys_deleted = 0;
            for(const auto &kv: key_values) {
                if(kv.size() <= 1) {
                    continue;
                }
                std::unique_ptr<btree::Key> key = std::make_unique<kvdb::btree::Key>(kv.at(0), kv.at(1));
                tree->root = btree::Node::delete_key(tree->root, key.get(), &count_keys_deleted, stream_tree.get());
            }
            if(count_keys_deleted == 0) {
                PRINT("%s", "No rows deleted");
            } else {
                PRINT("%u row%s deleted", count_keys_deleted, (count_keys_deleted > 1 ? "s" : ""));
            }*/
            status = kvdb::OK;
        }
        return status;
    }

    void Table::display_found_rows(const std::vector<std::vector<KeyValue>> &found_rows) {
        if(found_rows.empty()) {
            return;
        }
        std::string str = std::string();
        std::string row = std::string();
        for(const auto &found_row : found_rows) {
            if(found_row.empty()) {
                continue;
            }
            row = "{";
            for(size_t j = 0; j < found_row.size(); j++) {
                row += found_row.at(j).key + ": ";
                row += found_row.at(j).value;
                if(j + 1 < found_row.size()) {
                    row += ", ";
                }
            }
            row += "}";
            if(row.length() > 2) {
                if(!str.empty()) {
                    str += "\n";
                }
                str += row;
            }
        }
        PRINT("%s", str.c_str());
    }

} // namespace kvdb