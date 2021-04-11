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
        for(const auto & i : fields) {
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
                    //auto id = (uint32_t)std::stoi( key_value.at(1 ));
                    has_not_id_key = false;
                    break;
                }
            }
            if(has_not_id_key) {
                std::vector<std::string> v{"id", std::to_string(primary_key++)};
                key_values.insert(key_values.begin(), v);
            } else {
                ++primary_key;
            }
            uint32_t bytes_written = stream_data->write_string(Action::get_key_values_str(key_values));
            if(bytes_written == 0) {
                return status;
            }
            uint32_t stream_pos = stream_data->total_bytes - bytes_written;
            for(const auto &kv: key_values) {
                if(kv.size() <= 1) {
                    continue;
                }
                std::unique_ptr<kvdb::btree::Key> key = std::make_unique<kvdb::btree::Key>(kv.at(0), kv.at(1));
                key->value->stream_data_pos = stream_pos;
                if(key->serialize(stream_tree.get())) {
                    tree->root = tree->root->insert_key_to_leaf(std::move(key));
                }
            }
            status = kvdb::OK;
        } else if(action == Action::GET) {
            std::vector<btree::Key *> found_keys{};
            btree::Key *found_key = nullptr;
            for(const auto &kv: key_values) {
                if(kv.size() <= 1) {
                    continue;
                }
                std::unique_ptr<btree::Key> key = std::make_unique<kvdb::btree::Key>(kv.at(0), kv.at(1));
                btree::Node::search_key(tree->root, key.get(), found_key, false);
                btree::Node::found_keys_count(found_key, &found_keys);
                /*for(const auto &i : keys_found) {
                    count_keys_found(i, &keys_found_count, fields);
                }*/
            }
            if(found_keys.empty()) {
                PRINT("%s", "No rows found");
            } else {
                std::string search_fields = get_search_fields(found_keys);
                PRINT("%u row%s found", found_keys.size(), (found_keys.size() > 1 ? "s" : ""));
                PRINT("%s", search_fields.c_str());
            }
            status = kvdb::OK;
        } else if(action == Action::DELETE) {
            std::vector<btree::Key *> keys_found{};
            uint32_t count_keys_deleted = 0;
            for(const auto &kv: key_values) {
                if(kv.size() <= 1) {
                    continue;
                }
                std::unique_ptr<btree::Key> key = std::make_unique<kvdb::btree::Key>(kv.at(0), kv.at(1));
                //btree::Node::delete_key(tree->root, key.get(), &keys_found);
                tree->root = btree::Node::delete_key(tree->root, key.get(), &count_keys_deleted, stream_tree.get());
            }
            if(count_keys_deleted == 0) {
                PRINT("%s", "No rows deleted");
            } else {
                PRINT("%u row%s deleted", count_keys_deleted, (count_keys_deleted > 1 ? "s" : ""));
            }
            status = kvdb::OK;
        }
        return status;
    }

    std::string Table::get_search_fields(const std::vector<btree::Key *> &keys_found) const {
        std::string str = std::string();
        if(stream_data->opened()) {
            for(const auto &i: keys_found) {
                if(i == nullptr || !stream_data->seek(i->value->stream_data_pos)) {
                    continue;
                }
                uint32_t len = stream_data->read_uint();
                if(len > 0) {
                    std::string value = stream_data->read_string(len);
                    if(!value.empty()) {
                        std::vector<std::string> vec = Cli::split_string(value, '&');
                        std::string row = "{";
                        for(size_t j = 0; j < vec.size(); j++) {
                            std::vector<std::string> v = Cli::split_string(vec.at(j), '=');
                            if(v.size() > 1) {
                                row += v.at(0) + ": ";
                                row += v.at(1);
                                if(j + 1 < vec.size()) {
                                    row += ", ";
                                }
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
                }
            }
        }
        return str;
    }

} // namespace kvdb