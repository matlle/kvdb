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

    int8_t TableQuery::get_action(const std::string &str) {
        if (str == "put") {
            return PUT_;
        }
        if (str == "get") {
            return GET_;
        }
        if (str == "delete") {
            return DELETE_;
        }
        return -1;
    }

    std::unique_ptr<TableQuery> TableQuery::get_table_query(const std::string &query) {
        std::unique_ptr<TableQuery> table_query = std::make_unique<TableQuery>();
        std::vector<std::string> words = Cli::split_string(query, '.');
        if(words.empty() || words.size() <= 1) {
            return nullptr;
        }
        table_query->table_name = words.at(0);
        std::vector<std::string> words1 = Cli::split_string(words.at(1), '(');
        if(words1.empty() || words1.size() <= 1) {
            return nullptr;
        }
        table_query->op = TableQuery::get_action(words1.at(0));
        std::string action_fields = words1.at(1).substr(0, words1.at(1).size() - 1);
        std::vector<std::string> fields = Cli::split_string(action_fields, ',');
        for(const auto &i : fields) {
            std::vector<std::string> field = Cli::split_string(i, '=');
            if(field.size() > 1 && !field.at(0).empty() && !field.at(1).empty()) {
                table_query->key_values.push_back(field);
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
                    table_query->order_by += words.at(2).at(i);
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
                        table_query->order = order;
                    }
                }
            }
        }
        return table_query;
    }

    Row::Row(const std::string &row_id) {
        this->row_id = row_id;
    }

    bool Row::get_stream(const std::string &path, const char *mode) {
        stream_data = std::make_unique<Stream>(path + "r" + row_id, mode);
        if(!stream_data->opened()) {
            if(strcmp(mode, O_APPEND) == 0) {
                PRINT_ERROR("%s", "can't open stream_data to append");
            }
            return false;
        }
        stream_tree = std::make_unique<Stream>(path + std::to_string(btree::Node::hash_key("r" + row_id)), mode);
        if(!stream_tree->opened()) {
            if(strcmp(mode, O_APPEND) == 0) {
                PRINT_ERROR("%s", "can't open stream_tree to append");
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
            PRINT_ERROR("failed to seek at beginning of stream_data", nullptr);
            return data;
        }

        std::string id_value = std::string();
        uint32_t len = 0;
        while((len = stream_data->read_uint()) > 0) {
            std::string key = stream_data->read_string(len);
            std::string value = std::string();
            if(!key.empty() && (len = stream_data->read_uint()) > 0) {
                value = stream_data->read_string(len);
                if(key == "id") {
                    id_value = value;
                    continue;
                }
                data[key] = value;
            }
        }
        if(!id_value.empty()) {
            data["id"] = id_value;
        }
        if(!stream_data->seek_end()) {
            PRINT_ERROR("%", "failed to seek_end of stream_data");
        }
        return data;
    }

    bool Row::has_value(const btree::Key *key, const std::vector<std::string> &kv) const {
        uint32_t len = 0;
        if(stream_data == nullptr || stream_tree == nullptr) {
            return false;
        }
        if(!stream_data->seek(key->stream_data_pos)) {
            PRINT_ERROR("failed to seek in stream_data", nullptr);
            return false;
        }

        if((len = stream_data->read_uint()) <= 0) {
            if(!stream_data->seek_end()) {
                PRINT_ERROR("failed to seek_end of stream_data", nullptr);
            }
            return false;
        }

        std::string str_key = stream_data->read_string(len);
        if(str_key.empty() || str_key != kv.at(0)) {
            if(!stream_data->seek_end()) {
                PRINT_ERROR("failed to seek_end of stream_data", nullptr);
            }
            return false;
        }

        if((len = stream_data->read_uint()) <= 0) {
            if(!stream_data->seek_end()) {
                PRINT_ERROR("failed to seek_end of stream_data", nullptr);
            }
            return false;
        }

        std::string str_value = stream_data->read_string(len);

        if(str_value.empty() || str_value != kv.at(1)) {
            if(!stream_data->seek_end()) {
                PRINT_ERROR("failed to seek_end of stream_data", nullptr);
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

    void Row::sort_found_rows(std::vector<std::unordered_map<std::string, std::string>> &found_rows, std::unique_ptr<TableQuery> action) {
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
        path = db_path + std::string((const char *)PATH_SEPARATOR) + name + std::string((const char *)PATH_SEPARATOR);
    }

    bool Table::open() {
        if(!Database::create_directory(path.c_str())) {
            return false;
        }
        if(stream_meta == nullptr) {
            stream_meta = std::make_unique<Stream>(path + "meta", O_APPEND);
        }
        return opened = true;
    }

    std::unique_ptr<Row> Table::get_row(const std::string &row_id, bool create_if_not_exists) {
        bool stream_row_does_not_exists = true;
        if(Stream::file_exists(std::string(path + std::to_string(btree::Node::hash_key("r" + row_id))).c_str())) {
            stream_row_does_not_exists = false;
        }
        std::unique_ptr<Row> row = std::make_unique<Row>(row_id);
        if(!row->get_stream(path, create_if_not_exists ? O_APPEND : O_READONLY)) {
            row.reset();
            return nullptr;
        }
        if(!row->stream_tree->seek(0)) {
            PRINT_ERROR("can't seek to start of stream_tree", nullptr);
            return nullptr;
        }
        row->tree = btree::BTree::deserialize(row->stream_tree.get());
        if(row->tree == nullptr) {
            PRINT_ERROR("failed to get tree object", nullptr);
            return nullptr;
        }
        if(!row->stream_tree->seek_end()) {
            PRINT_ERROR("can't seek_end stream_tree", nullptr);
            return nullptr;
        }

        if(stream_row_does_not_exists) {
            if(stream_meta != nullptr) {
                if(strcmp(stream_meta->mode, O_READONLY) == 0) {
                    stream_meta.reset();
                    stream_meta = std::make_unique<Stream>(path + "meta", O_APPEND);
                    if(!stream_meta->opened()) {
                        PRINT_ERROR("stream_meta not opened", nullptr);
                        return nullptr;
                    }
                }
                uint32_t bw = stream_meta->write_ushort(btree::Node::hash_key("r" + row_id));
            }
        }
        return row;
    }

    kvdb::Status Table::process_action(std::unique_ptr<TableQuery> action) {
        kvdb::Status status = kvdb::Status::ERROR_;
        if(action->op == TableQuery::PUT_) {
            std::string row_id = std::string();
            bool has_not_id = true;
            for(const auto &key_value : action->key_values) {
                if(!key_value.empty() && key_value.at(0) == "id") {
                    row_id = key_value.at(1);
                    has_not_id = false;
                    break;
                }
            }
            if(has_not_id) {
                if(stream_meta != nullptr && stream_meta->seek_end()) {
                    int64_t number_of_bytes = ftell(stream_meta->file_ptr);
                    max_id = (number_of_bytes / 2) + 1;
                }
                row_id = std::to_string(max_id);
            }

            std::unique_ptr<Row> row = nullptr;
            if(recent_row != nullptr && recent_row->row_id == row_id) {
                row = std::move(recent_row);
            } else {
                row = get_row(row_id);
            }
            if(row == nullptr) {
                PRINT_ERROR("failed to insert row", nullptr);
                return kvdb::Status::ERROR_;
            }

            uint32_t bytes_written = 0, stream_data_pos = 0;
            int64_t number_of_bytes = 0;
            std::unique_ptr<btree::Key> key = nullptr;
            if(has_not_id) {
                bytes_written = row->stream_data->write_string("id");
                bytes_written += row->stream_data->write_string(row_id);
                stream_data_pos = row->stream_data->total_bytes - bytes_written;
                key = std::make_unique<btree::Key>("id");
                key->stream_data_pos = stream_data_pos;
                number_of_bytes = ftell(row->stream_tree->file_ptr);
                if(number_of_bytes == 0) {
                    number_of_bytes = row->stream_tree->write_string(row_id);
                }
                if(key->serialize(row->stream_tree.get())) {
                    row->tree->root = row->tree->root->insert_key_to_leaf(std::move(key));
                }
            }
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
                key = std::make_unique<btree::Key>(kv.at(0));
                key->stream_data_pos = stream_data_pos;
                number_of_bytes = ftell(row->stream_tree->file_ptr);
                if(number_of_bytes == 0) {
                    number_of_bytes = row->stream_tree->write_string(row_id);
                }
                if(key->serialize(row->stream_tree.get())) {
                    row->tree->root = row->tree->root->insert_key_to_leaf(std::move(key));
                }
            }

            recent_row = std::move(row);
            status = kvdb::Status::OK_;
        } else if(action->op == TableQuery::GET_) {
            if(stream_meta == nullptr) {
                PRINT_ERROR("stream_meta null", nullptr);
                return status;
            }
            if(strcmp(stream_meta->mode, O_APPEND) == 0) {
                stream_meta.reset();
                stream_meta = std::make_unique<Stream>(path + "meta", O_READONLY);
                if(!stream_meta->opened()) {
                    PRINT_ERROR("failed to open stream_meta to read", nullptr);
                    return status;
                }
            }
            if(!stream_meta->seek(0)) {
                PRINT_ERROR("failed to seek at beginning of stream_meta", nullptr);
                return status;
            }
            std::vector<std::unordered_map<std::string, std::string>> found_rows{};
            std::vector<std::string> fields{};
            uint32_t slen = 0;
            std::string row_id = std::string();
            std::string stream_tree_path = std::string();
            std::unique_ptr<Stream> stream_tree = nullptr;
            uint16_t stream_tree_id = 0;
            while((stream_tree_id = stream_meta->read_ushort()) > 0) {
                stream_tree_path = path + std::to_string(stream_tree_id);
                if(!Stream::file_exists(stream_tree_path.c_str())) {
                    continue;
                }
                stream_tree = std::make_unique<Stream>(stream_tree_path, O_READONLY);
                if(!stream_tree->opened() || !stream_tree->seek(0)) {
                    PRINT_ERROR("can't open stream_tree to read", nullptr);
                    continue;
                }

                if((slen = stream_tree->read_uint()) <= 0 || (row_id = stream_tree->read_string(slen)).empty()) {
                    continue;
                }

                if(!Stream::file_exists(std::string(path + "r" + row_id).c_str())) {
                    continue;
                }

                std::unique_ptr<Row> row = get_row(row_id, false);
                if(row == nullptr) {
                    continue;
                }
                std::unordered_map<std::string, std::string> data = row->get_data(action->key_values, fields);
                row.reset();
                if(!data.empty()) {
                    found_rows.push_back(data);
                }
            }

            if(!stream_meta->seek(0)) {
                PRINT_ERROR("failed to seek at beginning of stream_meta", nullptr);
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
            status = kvdb::Status::OK_;
        } else if(action->op == TableQuery::DELETE_) {
            if(stream_meta == nullptr) {
                PRINT_ERROR("stream_meta null", nullptr);
                return status;
            }
            if(strcmp(stream_meta->mode, O_APPEND) == 0) {
                stream_meta.reset();
                stream_meta = std::make_unique<Stream>(path + "meta", O_READONLY);
                if(!stream_meta->opened()) {
                    PRINT_ERROR("failed to open stream_meta to read", nullptr);
                    return status;
                }
            }
            if(!stream_meta->seek(0)) {
                PRINT_ERROR("failed to seek at beginning of stream_meta", nullptr);
                return status;
            }

            uint32_t deleted_rows_count = 0;

            uint32_t slen = 0;
            std::string row_id = std::string();
            std::string stream_tree_path = std::string();
            std::unique_ptr<Stream> stream_tree = nullptr;
            uint16_t stream_tree_id = 0;
            while((stream_tree_id = stream_meta->read_ushort()) > 0) {
                stream_tree_path = path + std::to_string(stream_tree_id);
                if(!Stream::file_exists(stream_tree_path.c_str())) {
                    continue;
                }
                stream_tree = std::make_unique<Stream>(stream_tree_path, O_READONLY);
                if(!stream_tree->opened() || !stream_tree->seek(0)) {
                    PRINT_ERROR("can't open stream_tree to read", nullptr);
                    continue;
                }

                if((slen = stream_tree->read_uint()) <= 0 || (row_id = stream_tree->read_string(slen)).empty()) {
                    continue;
                }

                if(!Stream::file_exists(std::string(path + "r" + row_id).c_str())) {
                    continue;
                }

                std::unique_ptr<Row> row = get_row(row_id, false);
                if(row == nullptr) {
                    continue;
                }
                std::vector<btree::Key *> keys{};
                if(!action->key_values.empty()) {
                    if(row->has_keys_values(action->key_values, keys) && row->delete_row()) {
                        deleted_rows_count++;
                    }
                } else if(row->delete_row()) {
                    deleted_rows_count++;
                }
            }

            if(!stream_meta->seek(0)) {
                PRINT_ERROR("failed to seek at beginning of stream_meta", nullptr);
            }

            if(deleted_rows_count == 0) {
                PRINT("%s", "No rows deleted");
            } else {
                PRINT("%u row%s deleted", deleted_rows_count, (deleted_rows_count > 1 ? "s" : ""));
            }
            status = kvdb::Status::OK_;
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

            row = "{";
            std::unordered_map<std::string, std::string>::iterator it;
            size_t j = 0;
            std::string s = std::string();
            for(it = found_row.begin(); it != found_row.end(); ++it) {
                s = it->first + ": " + it->second;
                row += s;
                if(j + 1 < found_row.size()) {
                    row += ", ";
                }
                j++;
            }

            row += "}";

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