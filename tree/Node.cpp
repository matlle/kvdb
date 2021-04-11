/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <algorithm>
#include "Node.h"
#include "BTree.h"
#include "../utils/log.hpp"

namespace kvdb {

    namespace btree {

        Node::Node() = default;

        Node::Node(const std::string &key, const std::string &value) {
            keys[0] = std::make_unique<Key>(key, value);
        }

        Node::Node(const size_t &hash, const std::string &value) {
            keys[0] = std::make_unique<Key>(hash, value);
        }


        size_t Node::hash_key(const std::string &key) {
            return std::hash<std::string>{}(key);
        }

        bool Node::is_leaf() {
            bool ret = true;
            for(const auto &i : children) {
                if(i != nullptr) {
                    ret = false;
                    break;
                }
            }
            return ret;
        }

        void Node::add_to_keys(std::unique_ptr<Key> key) {
            for(auto &i : keys) {
                if(i == nullptr) {
                    i = std::move(key);
                    break;
                } else if(i->hash == key->hash/* && i->key != "id"*/) {
                    i->twins.push_back(std::move(key));
                    break;
                }
            }
        }

        std::unique_ptr<Node> Node::split_keys(std::unique_ptr<Key> key) {
            std::unique_ptr<Node> node = std::make_unique<Node>();
            int median = (int)std::floor(BTREE_MAX_DEGREE / 2);
            int i = 0;
            for(; i < median; i++) {
                node->keys[i] = std::move(keys[i]);
            }
            for(i = 0; i < BTREE_MAX_DEGREE - 1 - median - 1; i++) {
                keys[i] = std::move(keys[++median]);
            }
            add_to_keys(std::move(key));
            sort_keys();
            return node;
        }

        void Node::shift_children_to_left() {
            if(children.size() > BTREE_MAX_DEGREE) {
                for(size_t j = 0; j < children.size() / 2; j++) {
                    parent->children.at(0)->add_child_node(std::move(children.at(j)));
                }
                children.erase(children.begin(), children.begin() + (children.size() / 2));
            }
        }

        void Node::move_half_children_to_node(Node *node) {
            if(children.size() > BTREE_MAX_DEGREE) {
                for(size_t j = 0; j < children.size() / 2; j++) {
                    node->add_child_node(std::move(children.at(j)));
                }
                children.erase(children.begin(), children.begin() + (children.size() / 2));
            }
        }

        int Node::find_merging_node_key_index(Node *node, bool merging_to_predecessor) {
            int index = -1;
            if(merging_to_predecessor) {
                for(int i = 0; i < parent->keys_count(); i++) {
                    if(keys[keys_count() -1]->hash <= parent->keys[i]->hash && parent->keys[i]->hash <= node->keys[0]->hash) {
                        index = i;
                        break;
                    }
                }
            } else {
                for(int i = 0; i < parent->keys_count(); i++) {
                    if(node->keys[node->keys_count() - 1]->hash <= parent->keys[i]->hash && parent->keys[i]->hash <= keys[0]->hash <= parent->keys[i]->hash) {
                        index = i;
                        break;
                    }
                }
            }
            return index;
        }

        void Node::merge_node(Node *node) {
            for(int i = keys_count(), j = 0; i < BTREE_MAX_DEGREE - 1 && j < node->keys_count(); i++, j++) {
                keys[i] = std::move(node->keys[j]);
            }
            sort_keys();
            delete node;
            node = nullptr;
        }

        Node *Node::split(std::unique_ptr<Key> key) {
            if(parent == nullptr) {
                //parent = std::make_shared<Node>();
                //parent = std::unique_ptr<Node>().get();
                parent = new Node();

                std::unique_ptr<Key> median_key = std::move(keys[(int)std::floor(BTREE_MAX_DEGREE / 2)]);

                std::unique_ptr<Node> node1 = split_keys(std::move(key));
                move_half_children_to_node(node1.get());
                parent->add_child_node(std::move(node1));

                //shift_children_to_left();

                parent->add_child_node(std::move(std::unique_ptr<Node>(this)));

                Node *node = parent->insert_key(std::move(median_key));
                /*if(node != parent) {
                    node->parent = parent;
                }*/
                //BTree::get_instance()->root = std::shared_ptr<Node>(parent);
                //return parent.get();
                //return parent;
                return node != parent ? node : parent;
            }
            std::unique_ptr<Key> median_key = std::move(keys[(int)std::floor(BTREE_MAX_DEGREE / 2)]);
            std::unique_ptr<Node> nn = split_keys(std::move(key));
            std::unique_ptr<Node> this_node = std::move(parent->children.at(parent->children.size() - 1));
            parent->children.pop_back();

            move_half_children_to_node(nn.get());
            parent->add_child_node(std::move(nn));

            parent->add_child_node(std::move(this_node));

            Node *node = parent->insert_key(std::move(median_key));

            //shift_children_to_left();

            return node != parent ? node : parent;
        }

        Node *Node::insert_key_to_leaf(std::unique_ptr<Key> key) {
            // we want to keep keys unique in nodes
            int key_found_index = 0;
            if(contains_key(key.get(), &key_found_index)) {
                keys[key_found_index]->twins.push_back(std::move(key));
                return BTree::find_root_node(this, parent);
            }

            // try to find the key in the entire tree
            std::vector<btree::Key *> found_keys{};
            btree::Node::find_key(this, key.get(), &found_keys, false);
            if(!found_keys.empty()) {
                found_keys.at(0)->twins.push_back(std::move(key));
                return BTree::find_root_node(this, parent);
            }

            if(is_leaf()) {
                Node *node = insert_key(std::move(key));
                return BTree::find_root_node(node, node->parent);
            }

            Node *node = this;
            bool was_broke = false;
            do {
                node = find_child_node(key.get(), node);
                if(node == nullptr) {
                    break;
                }
                if(node->contains_key(key.get(), &key_found_index)) {
                    node->keys[key_found_index]->twins.push_back(std::move(key));
                    was_broke = true;
                    break;
                }
            } while (!node->is_leaf());
            if(was_broke) {
                return BTree::find_root_node(node, node->parent);
            }
            if(node == nullptr) {
                //return this;
                return BTree::find_root_node(this, parent);
            }
            Node *node1 = node->insert_key(std::move(key));
            return BTree::find_root_node(node1, node1->parent);
        }

        Node *Node::insert_key(std::unique_ptr<Key> key) {
            if(keys_count() + 1 == BTREE_MAX_DEGREE) { // node is full
                return split(std::move(key));
            } else {
                add_to_keys(std::move(key));
            }
            sort_keys();
            return this;
        }

        int Node::binary_search(const Key *key) {
            int key_found_index = -1;
            int median;
            int left_index = 0;
            int right_index = keys_count() - 1;
            while(left_index <= right_index) {
                median = (int)std::floor((left_index + right_index) / 2);
                if(keys[median] == nullptr) {
                    break;
                }
                if(keys[median]->hash < key->hash) {
                    left_index = median + 1;
                } else if(keys[median]->hash > key->hash) {
                    right_index = median - 1;
                } else {
                    key_found_index = median;
                    break;
                }
            }
            return key_found_index;
        }

        int Node::contains_key(const Key *key, int *found_key_index) {
            return (*found_key_index = binary_search(key)) > -1 && *found_key_index < BTREE_MAX_DEGREE && keys[*found_key_index] != nullptr;
        }

        void Node::search_key(Node *&node, const Key *key, Key *&found_key, bool searched) {
            if(node == nullptr || key == nullptr) {
                return;
            }
            if(!searched) {
                int key_found_index = 0;
                if(node->contains_key(key, &key_found_index)) {
                    found_key = node->keys[key_found_index].get();
                    return;
                }
            }

            if(!node->children.empty()) {
                found_key = search_key_in_children(node, key);
                if(found_key == nullptr) {
                    // node is now a child node
                    search_key(node, key, found_key, true);
                }
            }
        }

        void Node::find_key(Node *node, const Key *key, std::vector<kvdb::btree::Key *> *found_keys, bool searched) {
            if(node == nullptr || key == nullptr) {
                return;
            }
            if(!searched) {
                int key_found_index = 0;
                if(node->contains_key(key, &key_found_index)) {
                    found_keys->push_back(node->keys[key_found_index].get());
                    return;
                }
            }
            if(!node->children.empty()) {
                Key *key_found = search_key_in_children(node, key);
                if(key_found == nullptr) {
                    // node is now a child node
                    find_key(node, key, found_keys, true);
                } else {
                    found_keys->push_back(key_found);
                }
            }
        }

        Node *Node::find_child_node(const Key *key, const Node *node) {
            Node *child = nullptr;
            for(int64_t i = node->children.size() - 1; i >= 0; i--) {
                for(int j = node->children.at(i)->keys_count() - 1; j >= 0; j--) {
                    if(node->children.at(i)->keys[j]->hash > key->hash) {
                        continue;
                    }
                    child = node->children.at(i).get();
                    break;
                }
                if(child != nullptr) {
                    break;
                }
                if(i == 0) {
                    child = node->children.at(0).get();
                }
            }
            return child;
        }

        Key *Node::search_key_in_children(Node *&node, const Key *search_key) {
            int key_found_index = -1;
            Key *key_found = nullptr;
            for(size_t i = 0; i < node->children.size(); i++) {
                if(node->children.at(i)->contains_key(search_key, &key_found_index)) {
                    key_found = node->children.at(i)->keys[key_found_index].get();
                    node = node->children.at(i).get();
                    break;
                } else if((search_key->hash <= node->children.at(i)->keys[0]->hash) || node->children.at(i)->keys[0]->hash < search_key->hash && search_key->hash <= node->children.at(i)->keys[0]->hash) {
                    node = node->children.at(i).get();
                    break;
                }
                if(i + 1 == node->children.size()) {
                    node = node->children.at(i).get();
                    break;
                }
            }
            return key_found;
        }

        void Node::delete_key(Node *node, Key *key, std::vector<kvdb::btree::Key *> *found_keys) {
            btree::Key *found_key = nullptr;
            search_key(node, key, found_key, false);
            if(found_key == nullptr) {
                return;
            }
            key->deleted = true;
            insert_into_found_keys(key, found_keys);
        }

        Node *Node::delete_key(Node *&node, Key *key, uint32_t *count_keys_deleted, Stream *stream) {
            btree::Key *found_key = nullptr;
            search_key(node, key, found_key, false);
            if(found_key == nullptr) {
                return BTree::find_root_node(node, node->parent);
            }
            if(stream != nullptr && !key->serialize_deleted(stream)) {
                return BTree::find_root_node(node, node->parent);
            }
            std::vector<btree::Key *> found_keys{};
            found_keys_count(found_key, &found_keys);
            *count_keys_deleted = found_keys.size();

            if(node->is_leaf() && node->parent == nullptr) { // root
                if(node->remove_key(found_key) > -1) {
                    node->sort_keys();
                }
                return node;
            } else if(node->is_leaf()) {
                if(node->has_more_keys()) {
                    if(node->remove_key(found_key) > -1) {
                        node->sort_keys();
                    }
                } else {
                    Node *predecessor_node = node->predecessor_node();
                    if(predecessor_node != nullptr && predecessor_node->has_more_keys()) {
                        int found_key_index = node->remove_key(found_key);
                        if(found_key_index > -1) {
                            std::unique_ptr<Key> predecessor_max_key = std::move(predecessor_node->keys[predecessor_node->keys_count() - 1]);
                            int parent_max_key_index = node->parent->keys_count() - 1;
                            std::unique_ptr<Key> parent_max_key = std::move(node->parent->keys[parent_max_key_index]);
                            node->parent->keys[parent_max_key_index] = std::move(predecessor_max_key);
                            node->keys[found_key_index] = std::move(parent_max_key);
                            node->sort_keys();
                        }
                        return BTree::find_root_node(node, node->parent);
                    }
                    Node *successor_node = node->successor_node();
                    if(successor_node != nullptr && successor_node->has_more_keys()) {
                        int found_key_index = node->remove_key(found_key);
                        if(found_key_index > -1) {
                            std::unique_ptr<Key> successor_min_key = std::move(successor_node->keys[0]);
                            int parent_max_key_index = node->parent->keys_count() - 1;
                            std::unique_ptr<Key> parent_max_key = std::move(node->parent->keys[parent_max_key_index]);
                            node->parent->keys[parent_max_key_index] = std::move(successor_min_key);
                            node->keys[found_key_index] = std::move(parent_max_key);
                            node->sort_keys();
                        }
                        return BTree::find_root_node(node, node->parent);
                    }
                    if(predecessor_node != nullptr && predecessor_node->parent != nullptr) {
                        int merging_nodes_key_index = predecessor_node->find_merging_node_key_index(node);
                        if(merging_nodes_key_index == -1) {
                            return BTree::find_root_node(predecessor_node, predecessor_node->parent);
                        }
                        int found_key_index = predecessor_node->remove_key(found_key);
                        if(found_key_index > -1) {
                            predecessor_node->merge_node(node);
                            int parent_keys_count = predecessor_node->parent->keys_count();
                            predecessor_node->keys[predecessor_node->keys_count()] = std::move(predecessor_node->parent->keys[merging_nodes_key_index]);
                            predecessor_node->parent->move_key_at_index(merging_nodes_key_index, parent_keys_count);
                            predecessor_node->parent->sort_keys();
                            predecessor_node->sort_keys();
                        }
                        return BTree::find_root_node(predecessor_node, predecessor_node->parent);
                    } else if(successor_node != nullptr && successor_node->parent != nullptr) {
                        int merging_nodes_key_index = successor_node->find_merging_node_key_index(node, false);
                        if(merging_nodes_key_index == -1) {
                            return BTree::find_root_node(successor_node, successor_node->parent);
                        }
                        int found_key_index = node->remove_key(found_key);
                        if(found_key_index > -1) {
                            successor_node->merge_node(node);
                            int parent_keys_count = successor_node->parent->keys_count();
                            successor_node->keys[successor_node->keys_count()] = std::move(successor_node->parent->keys[merging_nodes_key_index]);
                            successor_node->parent->move_key_at_index(merging_nodes_key_index, parent_keys_count);
                            successor_node->parent->sort_keys();
                            successor_node->sort_keys();
                        }
                        return BTree::find_root_node(successor_node, successor_node->parent);
                    }
                }
            } else if(!node->is_leaf() && node->parent != nullptr) {
                //
            }
            return BTree::find_root_node(node, node->parent);
        }

        Node *Node::predecessor_node() {
            Node *predecessor_node = nullptr;
            if(parent == nullptr) {
                return predecessor_node;
            }
            for(int i = 0; i < parent->children.size(); i++) {
                if(parent->children.at(i).get() == this && i - 1 >= 0) {
                    predecessor_node = parent->children.at(i - 1).get();
                    break;
                }
            }
            return predecessor_node;
        }

        Node *Node::successor_node() {
            Node *successor_node = nullptr;
            if(parent == nullptr) {
                return successor_node;
            }
            for(int i = 0; i < parent->children.size(); i++) {
                if(parent->children.at(i).get() == this && i + 1 < parent->children.size()) {
                    successor_node = parent->children.at(i + 1).get();
                    break;
                }
            }
            return successor_node;
        }

        int Node::remove_key(const Key *key) {
            int index = -1, i = 0, old_count = keys_count();
            for(; i < old_count; i++) {
                if(keys[i].get() == key) {
                    keys[i].reset();
                    index = i;
                    break;
                }
            }
            move_key_at_index(index, old_count);
            return index;
        }

        void Node::move_key_at_index(int index, int old_count) {
            if(index > -1) {
                bool moved = false;
                for(int i = 0; i < old_count; i++) {
                    if(keys[i] != nullptr) {
                        keys[index] = std::move(keys[i]);
                        moved = true;
                        break;
                    }
                }
                if(moved) {
                    sort_keys();
                }
            }
        }

        void Node::found_keys_count(Key *key, std::vector<kvdb::btree::Key *> *keys) {
            if(key == nullptr) {
                return;
            }
            insert_into_found_keys(key, keys);
            for(const auto &twin_key : key->twins) {
                found_keys_count(twin_key.get(), keys);
            }
        }

        void Node::insert_into_found_keys(Key *key, std::vector<kvdb::btree::Key *> *found_keys) {
            if(std::find(found_keys->begin(), found_keys->end(), key) == found_keys->end()) {
                bool not_duplicate = true;
                for(const auto &i : *found_keys) {
                    if(i->value->stream_data_pos == key->value->stream_data_pos) {
                        not_duplicate = false;
                        break;
                    }
                }
                if(not_duplicate) {
                    found_keys->push_back(key);
                }
            }
        }

        void Node::sort_keys() {
            // insertion sort
            int i = 1;
            while(i < BTREE_MAX_DEGREE - 1) {
                int j = i;
                while(j > 0 && keys[j - 1] != nullptr && keys[j] != nullptr && keys[j - 1]->hash > keys[j]->hash) {
                    std::swap(keys[j], keys[j - 1]);
                    j--;
                }
                i++;
            }
        }

        void Node::add_child_node(std::unique_ptr<Node> node) {
            if(node->parent == nullptr || node->parent != this) {
                node->parent = this;
                //node->parent = std::shared_ptr<std::remove_cv_t<Node>>(*this, const_cast<std::remove_cv_t<Node>*>(this));
            }
            bool exists = false;
            for(auto & i : children) {
                if(i.get() == node.get()) {
                    exists = true;
                    break;
                }
            }
            if(!exists) {
                children.push_back(std::move(node));
            }
        }

        int Node::keys_count() const {
            int count = 0;
            for(auto &key : keys) {
                if(key != nullptr) {
                    count++;
                }
            }
            return count;
        }

        bool Node::has_more_keys() const {
            return keys_count() > (int)std::ceil((BTREE_MAX_DEGREE - 1) / 2);
        }

    } // namespace btree

} // namespace kvdb


