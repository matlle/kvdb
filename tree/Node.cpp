/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
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

        Node *Node::split(std::unique_ptr<Key> key) {
            if(parent == nullptr) {
                //parent = std::make_shared<Node>();
                //parent = std::unique_ptr<Node>().get();
                parent = new Node();

                std::unique_ptr<Key> median_key = std::move(keys[(int)std::floor(BTREE_MAX_DEGREE / 2)]);

                parent->add_child_node(split_keys(std::move(key)));

                shift_children_to_left();

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
            parent->add_child_node(std::move(nn));
            parent->add_child_node(std::move(this_node));

            Node *node = parent->insert_key(std::move(median_key));
            /*if(node != parent) {
                //node->parent = parent;
                node->parent = parent->parent;
            }*/

            shift_children_to_left();

            //return parent.get();
            //return parent;
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
            std::vector<btree::Key *> keys_found{};
            btree::Node::find_key(this, key.get(), &keys_found, false);
            if(!keys_found.empty()) {
                keys_found.at(0)->twins.push_back(std::move(key));
                return BTree::find_root_node(this, parent);
            }

            if(is_leaf()) {
                Node *node = insert_key(std::move(key));
                return BTree::find_root_node(node, node->parent);
            }

            Node *node = this;
            bool was_broke = false;
            do {
                /*int keys_count = node->keys_count();
                if(key->hash <= node->keys[0]->hash) {
                    node = node->children.at(0).get();
                } else if(key->hash > node->keys[keys_count - 1]->hash) {
                    node = node->children.at(node->children.size() - 1).get();
                } else if(key->hash > node->keys[0]->hash && key->hash < node->keys[keys_count - 1]->hash) {
                    for(int i = 0; i < keys_count; i++) {
                        if(key->hash < node->keys[i]->hash) {
                            node = node->children.at(i).get();
                            break;
                        } else if(key->hash > node->keys[i]->hash && i + 1 < keys_count && key->hash < node->keys[i + 1]->hash && i + 1 < node->children.size()) {
                            node = node->children.at(i + 1).get();
                            break;
                        }
                    }
                }*/
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
                    /*if(keys[median]->value->data == key->value->data) {
                        found_keys->push_back(node->keys[median].get());
                        for(auto &key : node->keys[median]->siblings) {
                            found_keys->push_back(key.get());
                        }
                    }
                    key_found = true;*/
                    key_found_index = median;
                    break;
                }
            }
            return key_found_index;
        }

        int Node::contains_key(const Key *key, int *key_found_index) {
            return (*key_found_index = binary_search(key)) > -1 && *key_found_index < BTREE_MAX_DEGREE && keys[*key_found_index] != nullptr;
        }

        void Node::find_key(Node *node, Key *search_key, std::vector<kvdb::btree::Key *> *found_keys, bool searched) {
            if(node == nullptr || search_key == nullptr) {
                return;
            }
            if(!searched) {
                int key_found_index = 0;
                if(node->contains_key(search_key, &key_found_index)) {
                    found_keys->push_back(node->keys[key_found_index].get());
                    return;
                }
            }
            if(!node->children.empty()) {
                Key *key_found = find_key_in_children(node, search_key);
                if(key_found == nullptr) {
                    // node is now a child node
                    find_key(node, search_key, found_keys, true);
                } else {
                    found_keys->push_back(key_found);
                }
            }


            //int key_found_index = node->binary_search(search_key);
            //if(key_found_index > -1 && node->keys[key_found_index] != nullptr) {
                /*if(node->keys[key_found_index]->value->data == search_key->value->data) {
                    found_keys->push_back(node->keys[median].get());
                    for(auto &key : node->keys[median]->siblings) {
                        found_keys->push_back(key.get());
                    }
                }*/
            //    found_keys->push_back(node->keys[key_found_index].get());
            //    key_found = true;
            //}

            /*int keys_count = node->keys_count();
            // binary search
            int median;
            int left_index = 0;
            int right_index = keys_count - 1;
            while(left_index <= right_index) {
                median = (int)std::floor((left_index + right_index) / 2);
                if(node->keys[median] == nullptr) {
                    break;
                }
                if(node->keys[median]->hash < search_key->hash) {
                    left_index = median + 1;
                } else if(node->keys[median]->hash > search_key->hash) {
                    right_index = median - 1;
                } else {
                    if(node->keys[median]->value->data == search_key->value->data) {
                        found_keys->push_back(node->keys[median].get());
                        for(auto &key : node->keys[median]->siblings) {
                            found_keys->push_back(key.get());
                        }
                    }
                    key_found = true;
                    break;
                }
            }*/

            /*if(!is_key_found && !node->children.empty()) {
                node = find_child_node(search_key, node, false);
                if(node != nullptr) {
                    find_key(node, search_key, found_keys);
                }
            }*/
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

            /*for(size_t i = 0; i < parent->children.size(); i++) {
                for(int j = 0; j < parent->children.at(i)->keys_count(); j++) {
                    if(key->hash > parent->children.at(i)->keys[j]->hash) {
                        continue;
                    }
                    child = parent->children.at(i).get();
                    break;
                }
                if(child != nullptr) {
                    break;
                }
                if(i + 1 == parent->children.size()) {
                    child = parent->children.at(parent->children.size() - 1).get();
                }
            }*/

            /*for(const auto &i : node->children) {
                if(next_node != nullptr) {
                    break;
                }
                for(int j = 0; j < i->keys_count(); j++) {
                    if(i->keys[j]->hash > key->hash) {
                        continue;
                    }
                    next_node = i.get();
                    break;
                }
            }*/
            /*Node *next_node = nullptr;
            int keys_count = node->keys_count();
            if(key->hash <= node->keys[0]->hash) {
                next_node = node->children.at(0).get();
            } else if(key->hash > node->keys[keys_count - 1]->hash) {
                next_node = node->children.at(node->children.size() - 1).get();
            } else if(key->hash > node->keys[0]->hash && key->hash < node->keys[keys_count - 1]->hash) {
                for(int i = 0; i < keys_count; i++) {
                    if(key->hash < node->keys[i]->hash) {
                        next_node = node->children.at(i).get();
                        break;
                    } else if(key->hash > node->keys[i]->hash && i + 1 < keys_count && key->hash < node->keys[i + 1]->hash && i + 1 < node->children.size()) {
                        next_node = node->children.at(i + 1).get();
                        break;
                    }
                }
            }
            return next_node;*/
        }

        Key *Node::find_key_in_children(Node *&node, const Key *search_key) {
            int key_found_index = -1;
            Key *key_found = nullptr;
            for(size_t i = 0; i < node->children.size(); i++) {
                if(node->children.at(i)->contains_key(search_key, &key_found_index)) {
                    key_found = node->children.at(i)->keys[key_found_index].get();
                    break;
                } else if(search_key->hash < node->children.at(i)->keys[0/*node->children.at(i)->keys_count() - 1*/]->hash) {
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

    } // namespace btree

} // namespace kvdb


