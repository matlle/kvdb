/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
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
        for(auto & i : fields) {
            //std::vector<std::string> field = Cli::split_string(i, ':');
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
        /*if(has_not_id) {
            str = "id=" + std::to_string(Table::primary_key++) + "&" + str;
        } else if(!str.empty()) {
            ++Table::primary_key;
        }*/
        return str;
    }

    Table::Table(const std::string &name, const std::string &db_path) {
        stream_tree = std::make_unique<Stream>(db_path + "/" + std::to_string(std::hash<std::string>{}(name)), O_AWRITE);
        if(!stream_tree->opened()) {
            ERROR("%s", "can't open stream_tree");
            return;
        }
        if(!stream_tree->seek(0)) {
            ERROR("%s", "can't seek to start of stream_tree");
            return;
        }
        tree = btree::BTree::deserialize(stream_tree.get());
        if(tree == nullptr) {
            ERROR("%s", "failed to get tree object");
            return;
        }
        if(!stream_tree->seek_end()) {
            ERROR("%s", "can't seek_end stream_tree");
            return;
        }
        stream_data = std::make_unique<Stream>(db_path + "/" + name, O_AWRITE);
        if(!stream_data->opened()) {
            ERROR("%s", "can't open stream_data");
            return;
        }
    }

    kvdb::Status Table::process_action(int8_t action, std::vector<std::vector<std::string>> &key_values, std::vector<std::vector<std::string>> *result) {
        if(!stream_data->opened()) {
            return kvdb::ERROR;
        }
        kvdb::Status status = kvdb::ERROR;
        if(action == Action::PUT) {
            bool has_not_id_key = true;
            for(const auto &key_value : key_values) {
                if(!key_value.empty() && key_value.at(0) == "id") {
                    has_not_id_key = false;
                    break;
                }
            }
            if(has_not_id_key) {
                std::vector<std::string> v{"id", std::to_string(Table::primary_key++)};
                key_values.insert(key_values.begin(), v);
            } else {
                ++Table::primary_key;
            }
            uint32_t bytes_written = stream_data->write_string(Action::get_key_values_str(key_values));
            if(bytes_written == 0) {
                return status;
            }
            uint32_t file_pos = stream_data->total_bytes - bytes_written;
            for(auto kv: key_values) {
                if(kv.size() <= 1) {
                    continue;
                }
                std::unique_ptr<kvdb::btree::Key> key = std::make_unique<kvdb::btree::Key>(kv.at(0), kv.at(1));
                key->value->file_pos = file_pos;
                if(key->serialize(stream_tree.get())) {
                    tree->root = tree->root->insert_key_to_leaf(std::move(key));
                }
            }
            status = kvdb::OK;
        } else if(action == Action::GET) {
            std::vector<btree::Key *> keys_found{};
            uint32_t keys_found_count = 0;
            std::string fields = std::string();
            for(auto kv: key_values) {
                if(kv.size() <= 1) {
                    continue;
                }
                std::unique_ptr<btree::Key> key = std::make_unique<kvdb::btree::Key>(kv.at(0), kv.at(1));
                btree::Node::find_key(tree->root, key.get(), &keys_found, false);
                for(const auto &i : keys_found) {
                    count_keys_found(i, &keys_found_count, fields);
                }
            }
            if(keys_found_count == 0) {
                PRINT("%s", "No rows found");
            } else {
                PRINT("%u row%s found", keys_found_count, (keys_found_count > 1 ? "s" : ""));
                PRINT("%s", fields.c_str());
            }
            status = kvdb::OK;
        } else if(action == Action::DELETE) {
            status = kvdb::OK;
        }
        return status;
    }

    void Table::count_keys_found(const btree::Key *key, uint32_t *count, std::string &fields) {
        if(key == nullptr) {
            return;
        }
        (*count)++;
        std::vector<std::vector<std::string>> values{};
        if(stream_data->opened() && stream_data->seek(key->value->file_pos)) {
            uint32_t len = stream_data->read_uint();
            if(len > 0) {
                std::string value = stream_data->read_string(len);
                if(!value.empty()) {
                    std::vector<std::string> vec = Cli::split_string(value, '&');
                    std::string row = "{";
                    for(size_t i = 0; i < vec.size(); i++) {
                        std::vector<std::string> v = Cli::split_string(vec.at(i), '=');
                        if(v.size() > 1) {
                            row += v.at(0) + ": ";
                            row += v.at(1);
                            if(i + 1 < vec.size()) {
                                row += ", ";
                            }
                        }
                    }
                    row += "}";
                    if(row.length() > 2) {
                        if(!fields.empty()) {
                            fields += "\n";
                        }
                        fields += row;
                    }
                }
            }
        }
        for(const auto &twin_key : key->twins) {
            count_keys_found(twin_key.get(), count, fields);
        }
    }

} // namespace kvdb