/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "TreeNode.h"
#include "../utils/log.hpp"

namespace kvdb {

    namespace tree {

        uint16_t TreeNode::NEXT_NODE_ID = 1;

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
                    key->key = std::string();
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
                        key->value = std::string();
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
            return keys_count() == PAGE_SIZE;
            //return keys_count() == PAGE_SIZE - 1;
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

        kvdb::StatusEx TreeNode::on_root_node_full() {
            std::unique_ptr<tree::TreeNode> left_node = std::make_unique<tree::TreeNode>(table_path);
            std::unique_ptr<tree::TreeNode> rigth_node = std::make_unique<tree::TreeNode>(table_path);
            left_node->parent = this;
            rigth_node->parent = this;
            // move children to left and right node also
            int median = (int)std::floor(PAGE_SIZE / 2);
            uint32_t i = 0;
            for(; i < median; i++) {
                if(keys[i] != nullptr) {
                    left_node->keys[i] = std::move(keys[i]);
                    left_node->keys[i]->key_serialized = false;
                }
            }
            int count = PAGE_SIZE - median - 1;
            for(i = 0; i < count; i++) {
                if(++median < PAGE_SIZE) {
                    if(keys[median] != nullptr) {
                        rigth_node->keys[i] = std::move(keys[median]);
                        rigth_node->keys[i]->key_serialized = false;
                    }
                }
            }
            move_keys_to_front();
            bool node_is_leaf = is_leaf();
            if(!node_is_leaf) {
                uint32_t count_children = children.size();
                for(i = 0; i < (count_children / 2); i++) {
                    left_node->children.push_back(std::move(children.at(i)));
                }
                for(i = (count_children / 2); i < count_children; i++) {
                    rigth_node->children.push_back(std::move(children.at(i)));
                }
                children.clear();
            }
            children.push_back(std::move(left_node));
            children.push_back(std::move(rigth_node));
            if(!disk_write(true, true /*!node_is_leaf*/) || !children.at(0)->disk_write() || !children.at(1)->disk_write()) {
                return Error("node disk_write() failed");
            }
            return Success();
        }

        kvdb::StatusEx TreeNode::on_node_full(TreeNode *parent_node, int *child_node_index) {
            if(parent_node == nullptr) {
                return Error("parent_node is null");
            }
            if(*child_node_index == -1) {
                for(int i = 0; i < parent_node->children.size(); i++) {
                    if(parent_node->children.at(i).get() == this) {
                        *child_node_index = i;
                        break;
                    }
                }
            }

            std::unique_ptr<tree::TreeNode> new_node = std::make_unique<tree::TreeNode>(parent_node->table_path);
            new_node->parent = parent_node;
            // move children also
            int median = get_median_index();
            int count = PAGE_SIZE - median - 1;
            uint32_t i;
            for(i = 0; i < count; i++) {
                if(++median < PAGE_SIZE) {
                    if(keys[median] != nullptr) {
                        new_node->keys[i] = std::move(keys[median]);
                        new_node->keys[i]->key_serialized = false;
                    }
                }
            }

            StatusEx status = insert(parent_node->parent, parent_node, std::move(keys[get_median_index()]), true);
            if(status.is_error()) {
                return status;
            }

            std::vector<std::unique_ptr<TreeNode>> tmp_children{};
            uint32_t new_node_index = 0;
            if(*child_node_index + 1 < parent_node->children.size()) {
                for(i = *child_node_index + 1; i < parent_node->children.size(); i++) {
                    tmp_children.push_back(std::move(parent_node->children.at(i)));
                }
                parent_node->children.erase(parent_node->children.begin() + *child_node_index + 1, parent_node->children.end());
                parent_node->children.push_back(std::move(new_node));
                new_node_index = parent_node->children.size() - 1;
                for(i = 0; i < tmp_children.size(); i++) {
                    parent_node->children.push_back(std::move(tmp_children.at(i)));
                }
            } else {
                parent_node->children.push_back(std::move(new_node));
                new_node_index = parent_node->children.size() - 1;
            }
            *child_node_index = (int)new_node_index;
            if(!parent_node->disk_write(false, true) || !parent_node->children.at(new_node_index)->disk_write() || !disk_write(true)) {
                return Error("node disk_write() failed");
            }
            return Success();
        }

        kvdb::StatusEx TreeNode::insert(TreeNode *parent_node, TreeNode *node, std::shared_ptr<tree::TreeKey> treeKey, bool moving_from_child_to_parent) {
            if(node == nullptr) {
                return Error("insert node failed: node is null");
            }
            if(node->is_full()) {
                kvdb::StatusEx status;
                if(parent_node != nullptr) {
                    int child_nodex_index = -1;
                    status = node->on_node_full(parent_node, &child_nodex_index);
                } else {
                    status = node->on_root_node_full();
                }
                if(status.is_error()) {
                    return status;
                }
            }
            return tree::TreeNode::insert_not_full(parent_node, node, std::move(treeKey), moving_from_child_to_parent);
        }

        kvdb::StatusEx TreeNode::insert_not_full(TreeNode *parent_node, TreeNode *node, std::shared_ptr<tree::TreeKey> treeKey, bool moving_from_child_to_parent) {
            if(node->is_leaf() || moving_from_child_to_parent) {
                node->insert_into_keys(std::move(treeKey));
                node->sort_keys();
                if(node->is_full()) {
                    if(parent_node != nullptr) {
                        int child_nodex_index = -1;
                        node->on_node_full(parent_node, &child_nodex_index);
                    } else {
                        return node->on_root_node_full();
                    }
                }
                if(!node->disk_write(moving_from_child_to_parent)) {
                    return Error("node disk_write() failed");
                }
            } else {
                int child_node_index = -1;
                find_child_node(node, treeKey.get(), &child_node_index);
                //std::shared_ptr<TreeNode> child_node = disk_read(child_node_id, node->table_path);
                TreeNode *child_node = nullptr;
                if(child_node_index >= node->children.size() || node->children.at(child_node_index) == nullptr) {
                    return Error("failed to find child_node");
                }
                child_node = node->children.at(child_node_index).get();

                if(child_node->is_full()) {
                    uint16_t median_key_hash = child_node->keys[get_median_index()]->key_hash;
                    child_node->on_node_full(node, &child_node_index);
                    if(treeKey->key_hash < median_key_hash) {
                        insert_not_full(node, child_node, std::move(treeKey));
                    } else {
                        insert_not_full(node, node->children.at(child_node_index/*node->children.size() - 1*/).get(), std::move(treeKey));
                    }
                } else {
                    insert_not_full(node, child_node, std::move(treeKey));
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

        std::string TreeNode::disk_read_value(uint16_t value_hash) const {
            std::unique_ptr<Stream> value_stream = std::make_unique<Stream>(table_path + std::to_string(value_hash), O_READ);
            if(!value_stream->opened()) {
                return "";
            }
            uint32_t value_length = value_stream->read_uint();
            if(value_length <= 0 || value_length > DISK_READ_MAX) {
                return "";
            }
            return value_stream->read_string(value_length);
        }

        bool TreeNode::disk_read_children_node(bool lazy_read) {
            if(!children_stream_set_mode(O_READ)) {
                return false;
            }
            uint16_t node_id = 0;
            while((node_id = children_stream->read_ushort()) > 0) {
                std::unique_ptr<TreeNode> child_node = std::make_unique<TreeNode>(node_id, table_path);
                if(!lazy_read) {
                    child_node->disk_read_keys();
                    child_node->disk_read_children_node(false);
                }
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

        void TreeNode::find_child_node(TreeNode *node, const TreeKey *treeKey, int *child_node_index) {
            if(node->is_leaf()) {
                //*node_id = node->id;
                return;
            }
            uint16_t j = 0;
            while(j < node->keys_count() && (node->keys[j] != nullptr && treeKey->key_hash > node->keys[j]->key_hash)) {
                j++;
            }
            if(j >= node->children.size()) {
                j--;
            }
            if(j < 0) {
                j = 0;
            }
            *child_node_index = j;
            find_child_node(node->children.at(j).get(), treeKey, child_node_index);
        }

        kvdb::StatusEx TreeNode::get_child_node(TreeNode *node, const TreeKey *treeKey, TreeNode *&child_node) {
            int child_node_index = -1;
            find_child_node(node, treeKey, &child_node_index);
            if(child_node_index >= node->children.size() || node->children.at(child_node_index) == nullptr) {
                return Error("failed to find child_node");
            }
            child_node = node->children.at(child_node_index).get();
            return Success();
        }

        kvdb::StatusEx TreeNode::query(uint16_t node_id, std::unique_ptr<TreeKey> treeKey, std::string *value, const std::string &table_path) {
            std::shared_ptr<TreeNode> node = disk_read(node_id, table_path);
            if(node == nullptr) {
                return Success();
            }
            int found_key_index = -1;
            if(node->contains_key(treeKey->key_hash, &found_key_index)) {
                *value = node->disk_read_value(node->keys[found_key_index]->value_hash);
                return Success();
            }
            if(!node->is_leaf()) {
                TreeNode *child_node = nullptr;
                kvdb::StatusEx status = get_child_node(node.get(), treeKey.get(), child_node);
                if(status.is_error()) {
                    return status;
                }
                if(child_node == nullptr) {
                    return Error("ailed to find child_node");
                }
                return query(child_node->id, std::move(treeKey), value, table_path);
            }
            return Success();
        }

        int TreeNode::binary_search(const uint16_t key_hash) {
            int key_found_index = -1;
            int median;
            int left_index = 0;
            int right_index = keys_count() - 1;
            while(left_index <= right_index) {
                median = (int)std::floor((left_index + right_index) / 2);
                if(keys[median] == nullptr) {
                    break;
                }
                if(keys[median]->key_hash < key_hash) {
                    left_index = median + 1;
                } else if(keys[median]->key_hash > key_hash) {
                    right_index = median - 1;
                } else {
                    key_found_index = median;
                    break;
                }
            }
            return key_found_index;
        }

        int TreeNode::contains_key(const uint16_t key_hash, int *found_key_index) {
            return (*found_key_index = binary_search(key_hash)) > -1
                   && *found_key_index < PAGE_SIZE
                   && keys[*found_key_index] != nullptr
                   && !keys[*found_key_index]->deleted;
        }

        int TreeNode::get_median_index() {
            return (int)std::floor(PAGE_SIZE / 2);;
        }

    } // tree

} // kvdb
