/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "TreeNode.h"
#include "../utils/log.hpp"

namespace kvdb {

    namespace tree {

        uint16_t TreeNode::NEXT_NODE_ID = 0;

        TreeNode::TreeNode(const std::string &table_path) {
            this->id = NEXT_NODE_ID++;
            this->table_path = table_path;
            keys_stream_path = table_path + "n" + std::to_string(id);
            children_stream_path = table_path + "c" + std::to_string(id);
        }

        TreeNode::TreeNode(uint16_t id, const std::string &table_path) {
            this->id = id;
            this->table_path = table_path;
            keys_stream_path = table_path + "n" + std::to_string(id);
            children_stream_path = table_path + "c" + std::to_string(id);
        }

        std::shared_ptr<TreeNode> TreeNode::disk_read(uint16_t node_id, const std::string &table_path) {
            std::shared_ptr<TreeNode> node = std::make_shared<TreeNode>(node_id, table_path);
            if(node->disk_read_keys() && node->disk_read_children_node()) {
                return node;
            }
            return nullptr;
        }

        bool TreeNode::disk_write(bool keys_was_moved, bool children_was_moved) {
            if(keys_was_moved) {
                // delete the node keys_stream and rewrite it with remaining keys
                if(!delete_keys_stream()) {
                    return false;
                }
                for(auto &key: keys) {
                    if(key != nullptr) {
                        key->key_serialized = false;
                    }
                }
            }
            if(!keys_write()) {
                return false;
            }
            if(!is_leaf()) {
                if(children_was_moved && !delete_children_stream()) {
                    return false;
                }
                if(!children_write()) {
                    return false;
                }
            }
            return true;
        }

        bool TreeNode::keys_write() {
            if(!keys_stream_set_mode(O_WRITE)) {
                return false;
            }
            bool ret = true;
            for(auto &key: keys) {
                if(key != nullptr && !key->key_serialized) {
                    if(keys_stream->write_ushort(key->key_hash) == 0 || keys_stream->write_ushort(key->value_hash) == 0) {
                        ret = false;
                        break;
                    }
                    key->key_serialized = true;
                    if(!key->value_serialized) {
                        std::unique_ptr<Stream> value_stream = std::make_unique<Stream>(table_path + std::to_string(key->value_hash), O_WRITE);
                        if(!value_stream->opened()) {
                            ret = false;
                            break;
                        }
                        if(value_stream->write_string(key->value) == 0) {
                            ret = false;
                            break;
                        }
                        key->value_serialized = true;
                    }
                }
            }
            return ret;
        }

        bool TreeNode::children_write() {
            if(!children_stream_set_mode(O_WRITE)) {
                return false;
            }
            bool ret = true;
            for(auto &i: children) {
                if(i != nullptr) {
                    if(children_stream->write_ushort(i->id) == 0) {
                        ret = false;
                        break;
                    }
                }
            }
            return ret;
        }

        bool TreeNode::is_full() const {
            return keys_count() == PAGE_SIZE - 1;
        }

        bool TreeNode::is_leaf() const {
            return children.empty();
        }

        int TreeNode::keys_count() const {
            int count = 0;
            for(auto &key : keys) {
                if(key != nullptr) {
                    count++;
                }
            }
            return count;
        }

        void TreeNode::sort_keys() {
            // insertion sort
            int i = 1;
            while(i < PAGE_SIZE) {
                int j = i;
                while(j > 0 && keys[j - 1] != nullptr && keys[j] != nullptr && keys[j - 1]->key_hash > keys[j]->key_hash) {
                    std::swap(keys[j], keys[j - 1]);
                    j--;
                }
                i++;
            }
        }

        void TreeNode::insert_into_keys(std::shared_ptr<TreeKey> key) {
            for(auto &i : keys) {
                if(i == nullptr) {
                    i = std::move(key);
                    break;
                }
            }
        }

        void TreeNode::move_keys_to_front() {
            int empty_slot = -1;
            for(int i = 0; i < PAGE_SIZE; i++) {
                if(empty_slot < 0 && keys[i] == nullptr) {
                    empty_slot = i;
                    continue;
                }
                if(keys[i] != nullptr) {
                    keys[empty_slot] = std::move(keys[i]);
                    i = empty_slot;
                    empty_slot = -1;
                }
            }
        }

        bool TreeNode::delete_keys_stream() {
            if(keys_stream->close() && keys_stream->delete_file()) {
                keys_stream.reset();
                return true;
            }
            return false;
        }

        bool TreeNode::delete_children_stream() {
            if(children_stream->close() && children_stream->delete_file()) {
                children_stream.reset();
                return true;
            }
            return false;
        }

        kvdb::StatusEx TreeNode::insert(TreeNode *node, std::shared_ptr<tree::TreeKey> treeKey, bool moving_from_child_to_parent) {
            if(node == nullptr) {
                return Error("insert node failed: node is null");
            }
            if(node->is_full()) {
                std::unique_ptr<tree::TreeNode> left_node = std::make_unique<tree::TreeNode>(node->table_path);
                std::unique_ptr<tree::TreeNode> rigth_node = std::make_unique<tree::TreeNode>(node->table_path);
                // move children to left and right node also
                int median = (int)std::floor(PAGE_SIZE / 2);
                uint32_t i = 0;
                for(; i < median; i++) {
                    if(node->keys[i] != nullptr) {
                        left_node->keys[i] = std::move(node->keys[i]);
                        left_node->keys[i]->key_serialized = false;
                    }
                }
                int count = PAGE_SIZE - median - 1;
                for(i = 0; i < count; i++) {
                    if(++median < PAGE_SIZE) {
                        if(node->keys[median] != nullptr) {
                            rigth_node->keys[i] = std::move(node->keys[median]);
                            rigth_node->keys[i]->key_serialized = false;
                        }
                    }
                }
                node->move_keys_to_front();
                bool is_leaf = node->is_leaf();
                if(!is_leaf) {
                    uint32_t count_children = node->children.size();
                    for(i = 0; i < (count_children / 2); i++) {
                        left_node->children.push_back(std::move(node->children.at(i)));
                    }
                    for(i = (count_children / 2); i < count_children; i++) {
                        rigth_node->children.push_back(std::move(node->children.at(i)));
                    }
                }
                node->children.push_back(std::move(left_node));
                node->children.push_back(std::move(rigth_node));
                node->disk_write(true, !is_leaf);
                node->children.at(0)->disk_write();
                node->children.at(1)->disk_write();
            }
            return tree::TreeNode::insert_not_full(node, std::move(treeKey), moving_from_child_to_parent);
        }

        kvdb::StatusEx TreeNode::insert_not_full(TreeNode *node, std::shared_ptr<tree::TreeKey> treeKey, bool moving_from_child_to_parent) {
            if(node->is_leaf() || moving_from_child_to_parent) {
                node->insert_into_keys(std::move(treeKey));
                node->sort_keys();
                if(!node->disk_write()) {
                    return Error("root_node disk_write() failed");
                }
            } else {
                uint16_t child_node_id = 0;
                find_child_node(node, treeKey.get(), &child_node_id);
                //std::shared_ptr<TreeNode> child_node = disk_read(child_node_id, node->table_path);
                TreeNode *child_node = nullptr;
                if(child_node_id >= node->children.size() || node->children.at(child_node_id) == nullptr) {
                    return Error("failed to find child_node");
                }
                child_node = node->children.at(child_node_id).get();
                if(child_node->is_full()) {
                    std::unique_ptr<tree::TreeNode> new_node = std::make_unique<tree::TreeNode>(node->table_path);
                    // move children also
                    int median = (int)std::floor(PAGE_SIZE / 2);
                    int count = PAGE_SIZE - median - 1;
                    uint32_t i;
                    for(i = 0; i < count; i++) {
                        if(++median < PAGE_SIZE) {
                            if(child_node->keys[median] != nullptr) {
                                new_node->keys[i] = std::move(child_node->keys[median]);
                                new_node->keys[i]->key_serialized = false;
                            }
                        }
                    }
                    median = (int)std::floor(PAGE_SIZE / 2);
                    uint16_t median_key_hash = child_node->keys[median]->key_hash;
                    if(median < PAGE_SIZE) {
                        //node->insert_into_keys(std::move(child_node->keys[median]));
                        StatusEx status = insert(node, std::move(child_node->keys[median]), true);
                        if(status.is_error()) {
                            return status;
                        }
                    }
                    std::vector<std::unique_ptr<TreeNode>> tmp_children{};
                    uint32_t new_node_index = 0;
                    if(child_node_id + 1 < node->children.size()) {
                        for(i = child_node_id + 1; i < node->children.size(); i++) {
                            tmp_children.push_back(std::move(node->children.at(i)));
                        }
                        node->children.erase(node->children.begin() + child_node_id + 1, node->children.end());
                        node->children.push_back(std::move(new_node));
                        new_node_index = node->children.size() - 1;
                        for(i = 0; i < tmp_children.size(); i++) {
                            node->children.push_back(std::move(tmp_children.at(i)));
                        }
                    } else {
                        node->children.push_back(std::move(new_node));
                        new_node_index = node->children.size() - 1;
                    }
                    node->disk_write();
                    node->children.at(new_node_index)->disk_write();
                    child_node->disk_write(true);
                    if(treeKey->key_hash < median_key_hash) {
                        insert_not_full(child_node, std::move(treeKey));
                    } else {
                        insert_not_full(node->children.at(node->children.size() - 1).get(), std::move(treeKey));
                    }
                } else {
                    insert_not_full(child_node, std::move(treeKey));
                }
            }
            return Success();
        }

        bool TreeNode::disk_read_keys() {
            if(!keys_stream_set_mode(O_READ)) {
                return false;
            }
            uint16_t key_hash = 0;
            uint32_t i = 0;
            while((key_hash = keys_stream->read_ushort()) > 0 && !is_full()) {
                keys[i] = std::make_shared<TreeKey>();
                keys[i]->key_serialized = true;
                keys[i]->value_serialized = true;
                keys[i]->key_hash = key_hash;
                keys[i]->value_hash = keys_stream->read_ushort();
                i++;
            }
            sort_keys();
            return true;
        }

        bool TreeNode::disk_read_children_node(bool lazy_read) {
            if(!children_stream_set_mode(O_READ)) {
                return false;
            }
            uint16_t node_id = 0;
            while((node_id = children_stream->read_ushort()) > 0) {
                std::unique_ptr<TreeNode> child_node = std::make_unique<TreeNode>(node_id, table_path);
                children.push_back(std::move(child_node));
            }
            return true;
        }

        bool TreeNode::keys_stream_set_mode(const char *mode) {
            if(keys_stream != nullptr && strcmp(keys_stream->mode, mode) == 0) {
                return true;
            }
            if(keys_stream != nullptr) {
                keys_stream.reset();
            }
            if(!Stream::file_exists(keys_stream_path.c_str()) && !Stream::create_file(keys_stream_path.c_str())) {
                return false;
            }
            keys_stream = std::make_unique<Stream>(keys_stream_path, mode);
            if(!keys_stream->opened()) {
                keys_stream.reset();
                return false;
            }
            return true;
        }

        bool TreeNode::children_stream_set_mode(const char *mode) {
            if(children_stream != nullptr && strcmp(children_stream->mode, mode) == 0) {
                return true;
            }
            if(children_stream != nullptr) {
                children_stream.reset();
            }
            if(!Stream::file_exists(children_stream_path.c_str()) && !Stream::create_file(children_stream_path.c_str())) {
                return false;
            }
            children_stream = std::make_unique<Stream>(children_stream_path, mode);
            if(!children_stream->opened()) {
                children_stream.reset();
                return false;
            }
            return true;
        }

        void TreeNode::find_child_node(TreeNode *node, const TreeKey *treeKey, uint16_t *node_id) {
            if(node->is_leaf()) {
                *node_id = node->id;
                return;
            }
            uint16_t j = 0;
            while(j < node->keys_count() && (node->keys[j] != nullptr && treeKey->key_hash > node->keys[j]->key_hash)) {
                j++;
            }
            if(j >= node->children.size()) {
                j--;
            }
            j = j < 0 ? 0 : j;
            find_child_node(node->children.at(j).get(), treeKey, node_id);
        }

    } // tree

} // kvdb
