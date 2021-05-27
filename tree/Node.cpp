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

        uint16_t Node::hash_key(const std::string &key) {
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
                    if(keys[keys_count() - 1]->hash <= parent->keys[i]->hash && parent->keys[i]->hash <= node->keys[0]->hash) {
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

        void Node::merge_node(Node *node, bool merging_to_predecessor) {
            int node_keys_count = node->keys_count();
            for(int i = keys_count(), j = 0; i < BTREE_MAX_DEGREE - 1 && j < node_keys_count; i++, j++) {
                keys[i] = std::move(node->keys[j]);
            }
            sort_keys();
            if(merging_to_predecessor) {
                for(auto &i : node->children) {
                    i->parent = this;
                    children.push_back(std::move(i));
                }
            } else {
                for(int i = node->children.size() - 1; i >= 0; i--) {
                    node->children.at(i)->parent = this;
                    children.insert(children.begin(), std::move(node->children.at(i)));
                }
            }

            node->children.clear();
            if(parent != nullptr) {
                int node_index = -1;
                for(int i = 0; i < parent->children.size(); i++) {
                    if(parent->children.at(i).get() == node) {
                        node_index = i;
                        break;
                    }
                }
                if(node_index > -1) {
                    parent->children.erase(parent->children.begin() + node_index);
                    node = nullptr;
                }
            }
            //delete node;
            //node = nullptr;
        }

        Node *Node::split() {
            if(parent == nullptr) {

                parent = new Node();

                std::unique_ptr<Key> median_key = std::move(keys[(int)std::floor(BTREE_MAX_DEGREE / 2)]);

                std::unique_ptr<Node> node1 = split_keys();
                move_half_children_to_node(node1.get());
                parent->add_child_node(std::move(node1));

                parent->add_child_node(std::move(std::unique_ptr<Node>(this)));

                Node *node = parent->insert_key(std::move(median_key));

                return node != parent ? node : parent;
            }
            std::unique_ptr<Key> median_key = std::move(keys[(int)std::floor(BTREE_MAX_DEGREE / 2)]);
            std::unique_ptr<Node> nn = split_keys();
            int this_node_index = get_index_in_children();

            move_half_children_to_node(nn.get());
            parent->add_child_node(std::move(nn), this_node_index);

            Node *node = parent->insert_key(std::move(median_key));

            return node != parent ? node : parent;
        }

        void Node::add_to_keys(std::unique_ptr<Key> key) {
            for(auto &i : keys) {
                if(i == nullptr) {
                    i = std::move(key);
                    break;
                } else if(i->hash == key->hash) {
                    i->twins.push_back(std::move(key));
                    break;
                }
            }
        }

        std::unique_ptr<Node> Node::split_keys() {
            std::unique_ptr<Node> node = std::make_unique<Node>();
            int median = (int)std::floor(BTREE_MAX_DEGREE / 2);
            int i = 0;
            for(; i < median; i++) {
                if(keys[i] != nullptr) {
                    node->keys[i] = std::move(keys[i]);
                }
            }
            int count = BTREE_MAX_DEGREE - median - 1;
            for(i = 0; i < count; i++) {
                if(++median < BTREE_MAX_DEGREE) {
                    keys[i] = std::move(keys[median]);
                }
            }
            sort_keys();
            return node;
        }

        Node *Node::insert_key_to_leaf(std::unique_ptr<Key> key) {
            // to keep keys unique in nodes
            int key_found_index = 0;
            if(contains_key(key.get(), &key_found_index)) {
                keys[key_found_index].reset();
                keys[key_found_index] = std::move(key);
                return BTree::find_root_node(this, parent);
            }

            Node *node = this;

            if(is_leaf()) {
                node = insert_key(std::move(key));
                return BTree::find_root_node(node, node->parent);
            }

            bool was_break = false;
            do {
                node = find_child_node(key.get(), node);
                if(node == nullptr) {
                    was_break = true;
                    break;
                }
            } while (!node->is_leaf());
            if(was_break) {
                return BTree::find_root_node(this, parent);
            }
            Node *node1 = node->insert_key(std::move(key));
            return BTree::find_root_node(node1, node1->parent);
        }

        Node *Node::insert_key(std::unique_ptr<Key> key) {
            if(keys_count() < BTREE_MAX_DEGREE) {
                add_to_keys(std::move(key));
                sort_keys();
                if(keys_count() == BTREE_MAX_DEGREE) { // node is full
                    return split();
                }
            }
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
            return (*found_key_index = binary_search(key)) > -1 && *found_key_index < BTREE_MAX_DEGREE && keys[*found_key_index] != nullptr && !keys[*found_key_index]->deleted;
        }

        void Node::search_key(Node *&node, const Key *key, Key *&found_key) {
            int i = 0;
            while(i < node->keys_count() && (node->keys[i] == nullptr || key->hash > node->keys[i]->hash || node->keys[i]->deleted)) {
                i++;
            }
            if(i < node->keys_count() && key->hash == node->keys[i]->hash) {
                found_key = node->keys[i].get();
                return;
            }
            if(node->is_leaf()) {
                return;
            }
            if(i >= node->children.size()) {
                return;
            }
            node = node->children.at(i).get();
            search_key(node, key, found_key);
        }


        Node *Node::find_child_node(const Key *key, const Node *node) {
            Node *child = nullptr;
            for(int i = node->children.size() - 1; i >= 0; i--) {
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

        Node *Node::delete_key(Node *&found_node, Key *key, uint32_t *count_keys_deleted) {
            btree::Key *found_key = nullptr;
            if(count_keys_deleted != nullptr) {
                search_key(found_node, key, found_key);
                if(found_key == nullptr) {
                    return BTree::find_root_node(found_node, found_node->parent);
                }

                std::vector<btree::Key *> found_keys{};
                found_keys_count(found_key, &found_keys);
                *count_keys_deleted = found_keys.size();
            } else {
                found_key = key;
            }

            int old_keys_count = found_node->keys_count();
            if(found_node->is_leaf() && found_node->parent == nullptr) { // only node in the tree
                if(found_node->remove_key(found_key) > -1) {
                    found_node->move_keys_to_front(old_keys_count);
                    found_node->sort_keys();
                }
                return found_node;
            } else if(found_node->is_leaf()) {
                if(found_node->has_more_keys()) {
                    if(found_node->remove_key(found_key) > -1) {
                        found_node->move_keys_to_front(old_keys_count);
                        found_node->sort_keys();
                    }
                } else {
                    return remove_key_in_leaf_node_with_min_keys(found_key, found_node);
                }
            } else if(!found_node->is_leaf() && found_node->parent != nullptr) { // internal
                Node *predecessor_child_node = found_node->get_predecessor_child_node(found_key);
                if(predecessor_child_node != nullptr && predecessor_child_node->has_more_keys()) {
                    int found_key_index = found_node->remove_key(found_key);
                    if(found_key_index > -1) {
                        found_node->keys[found_key_index] = std::move(predecessor_child_node->keys[predecessor_child_node->keys_count() - 1]);
                    }
                    return BTree::find_root_node(predecessor_child_node, predecessor_child_node->parent);
                }
                Node *successor_child_node = found_node->get_successor_child_node(found_key);
                if(successor_child_node != nullptr && successor_child_node->has_more_keys()) {
                    int found_key_index = found_node->remove_key(found_key);
                    if(found_key_index > -1) {
                        int successor_keys_count = successor_child_node->keys_count();
                        found_node->keys[found_key_index] = std::move(successor_child_node->keys[0]);
                        successor_child_node->move_keys_to_front(successor_keys_count);
                    }
                    return BTree::find_root_node(successor_child_node, successor_child_node->parent);
                }
                if(predecessor_child_node != nullptr && successor_child_node != nullptr) {
                    old_keys_count = found_node->keys_count();
                    int found_key_index = found_node->remove_key(found_key);
                    if(found_key_index > -1) {
                        found_node->move_keys_to_front(old_keys_count);
                        found_node->sort_keys();
                        predecessor_child_node->merge_node(successor_child_node);
                        if(found_node->has_min_keys()) {
                            return BTree::find_root_node(found_node, found_node->parent);
                        }
                        check_parent_with_min_keys(found_node);
                    }
                    return BTree::find_root_node(predecessor_child_node, predecessor_child_node->parent);
                }
            } else if(found_node->is_root()) {
                Node *leaf_node_with_max_key = found_node->get_predecessor_child_node(found_key);
                while(!leaf_node_with_max_key->is_leaf()) {
                    leaf_node_with_max_key = leaf_node_with_max_key->children.at(leaf_node_with_max_key->children.size() - 1).get();
                }
                int found_key_index = found_node->remove_key(found_key);
                if(found_key_index > -1) {
                    found_node->keys[found_key_index] = std::move(leaf_node_with_max_key->keys[leaf_node_with_max_key->keys_count() - 1]);
                }
                if(!leaf_node_with_max_key->has_min_keys()) {
                    return remove_key_in_leaf_node_with_min_keys(nullptr, leaf_node_with_max_key);
                }
                return found_node;
            }
            return BTree::find_root_node(found_node, found_node->parent);
        }

        Node *Node::remove_key_in_leaf_node_with_min_keys(const Key *found_key, Node *node) {
            Node *predecessor_node = node->get_predecessor_node();
            if(predecessor_node != nullptr && predecessor_node->has_more_keys()) {
                predecessor_max_key_swap_with_parent_max_key(found_key, predecessor_node, node);
                return BTree::find_root_node(node, node->parent);
            }
            Node *successor_node = node->get_successor_node();
            if(successor_node != nullptr && successor_node->has_more_keys()) {
                successor_min_key_swap_with_parent_max_key(found_key, node, successor_node);
                return BTree::find_root_node(node, node->parent);
            }
            if(predecessor_node != nullptr && predecessor_node->parent != nullptr) {
                merge_node_to_predecessor(found_key, predecessor_node, node);
                return BTree::find_root_node(predecessor_node, predecessor_node->parent);
            }
            if(successor_node != nullptr && successor_node->parent != nullptr) {
                merge_node_to_successor(found_key, node, successor_node);
                return BTree::find_root_node(successor_node, successor_node->parent);
            }
            return BTree::find_root_node(node, node->parent);
        }

        void Node::merge_node_to_predecessor(const Key *found_key, Node *predecessor_node, Node *node) {
            int merging_nodes_key_index = predecessor_node->find_merging_node_key_index(node);
            if(merging_nodes_key_index == -1) {
                return;
            }
            if(found_key != nullptr) {
                int old_keys_count = node->keys_count();
                int index = node->remove_key(found_key);
                node->move_keys_to_front(old_keys_count);
                node->sort_keys();
                if(index <= -1) {
                    return;
                }
            }
            predecessor_node->merge_node(node);
            int parent_keys_count = predecessor_node->parent->keys_count();
            predecessor_node->keys[predecessor_node->keys_count()] = std::move(predecessor_node->parent->keys[merging_nodes_key_index]);
            predecessor_node->parent->move_keys_to_front(parent_keys_count);
            predecessor_node->parent->sort_keys();
            predecessor_node->sort_keys();
            if(predecessor_node->parent->is_root() && predecessor_node->parent->keys_count() == 0) {
                predecessor_node->parent = nullptr; // parent should be deleted
                return;
            }
            if(!predecessor_node->parent->is_root() && !predecessor_node->parent->has_min_keys()) {
                Node *parent_node = predecessor_node->parent;
                Node *predecessor_node1 = parent_node->get_predecessor_node();
                if(predecessor_node1 != nullptr && predecessor_node1->has_more_keys()) {
                    predecessor_max_key_swap_with_parent_max_key(nullptr, predecessor_node1, parent_node);
                }
                Node *successor_node1 = parent_node->get_successor_node();
                if(successor_node1 != nullptr && successor_node1->has_more_keys()) {
                    successor_min_key_swap_with_parent_max_key(nullptr, parent_node, successor_node1);
                }
                if(predecessor_node1 != nullptr && predecessor_node1->parent != nullptr) {
                    merge_node_to_predecessor(nullptr, predecessor_node1, parent_node);
                }
                if(successor_node1 != nullptr && successor_node1->parent != nullptr) {
                    merge_node_to_successor(nullptr, parent_node, successor_node1);
                }
            }
        }

        void Node::merge_node_to_successor(const Key *found_key, Node *node, Node *successor_node) {
            int merging_nodes_key_index = successor_node->find_merging_node_key_index(node, false);
            if(merging_nodes_key_index == -1) {
                return;
            }
            if(found_key != nullptr) {
                int old_keys_count = node->keys_count();
                int index = node->remove_key(found_key);
                node->move_keys_to_front(old_keys_count);
                node->sort_keys();
                if(index <= -1) {
                    return;
                }
            }
            successor_node->merge_node(node, false);
            int parent_keys_count = successor_node->parent->keys_count();
            successor_node->keys[successor_node->keys_count()] = std::move(successor_node->parent->keys[merging_nodes_key_index]);
            successor_node->parent->move_keys_to_front(parent_keys_count);
            successor_node->parent->sort_keys();
            successor_node->sort_keys();
            if(successor_node->parent->is_root() && successor_node->parent->keys_count() == 0) {
                successor_node->parent = nullptr; // parent should be deleted
                return;
            }
            if(!successor_node->parent->is_root() && !successor_node->parent->has_min_keys()) {
                Node *parent_node = successor_node->parent;
                Node *predecessor_node1 = parent_node->get_predecessor_node();
                if(predecessor_node1 != nullptr && predecessor_node1->has_more_keys()) {
                    predecessor_max_key_swap_with_parent_max_key(nullptr, predecessor_node1, parent_node);
                }
                Node *successor_node1 = parent_node->get_successor_node();
                if(successor_node1 != nullptr && successor_node1->has_more_keys()) {
                    successor_min_key_swap_with_parent_max_key(nullptr, parent_node, successor_node1);
                }
                if(predecessor_node1 != nullptr && predecessor_node1->parent != nullptr) {
                    merge_node_to_predecessor(nullptr, predecessor_node1, parent_node);
                }
                if(successor_node1 != nullptr && successor_node1->parent != nullptr) {
                    merge_node_to_successor(nullptr, parent_node, successor_node1);
                }
            }
        }

        void Node::check_parent_with_min_keys(Node *parent_node) {
            Node *predecessor_node = parent_node->get_predecessor_node();
            if(predecessor_node != nullptr && predecessor_node->has_more_keys()) {
                predecessor_max_key_swap_with_parent_max_key(nullptr, predecessor_node, parent_node);
            }
            Node *successor_node = parent_node->get_successor_node();
            if(successor_node != nullptr && successor_node->has_more_keys()) {
                successor_min_key_swap_with_parent_max_key(nullptr, parent_node, successor_node);
            }
            if(predecessor_node != nullptr && predecessor_node->parent != nullptr) {
                merge_node_to_predecessor(nullptr, predecessor_node, parent_node);
            }
            if(successor_node != nullptr && successor_node->parent != nullptr) {
                merge_node_to_successor(nullptr, parent_node, successor_node);
            }
        }

        void Node::predecessor_max_key_swap_with_parent_max_key(const Key *found_key, Node *predecessor_node, Node *node) {
            int found_key_index = -1;
            if(found_key != nullptr) {
                found_key_index = node->remove_key(found_key);
            }
            int old_keys_count = predecessor_node->keys_count();
            std::unique_ptr<Key> predecessor_max_key = std::move(predecessor_node->keys[old_keys_count - 1]);
            predecessor_node->move_keys_to_front(old_keys_count);
            int parent_max_key_index = node->parent->keys_count() - 1;
            std::unique_ptr<Key> parent_max_key = std::move(node->parent->keys[parent_max_key_index]);
            node->parent->keys[parent_max_key_index] = std::move(predecessor_max_key);
            if(found_key_index > -1) {
                node->keys[found_key_index] = std::move(parent_max_key);
                node->sort_keys();
            } else {
                node->keys[node->keys_count()] = std::move(parent_max_key);
            }
            if(found_key != nullptr && !predecessor_node->is_leaf() && !node->is_leaf()) {
                node->children.insert(node->children.begin(), std::move(std::unique_ptr<Node>(predecessor_node->get_successor_child_node(found_key))));
            }
        }

        void Node::successor_min_key_swap_with_parent_max_key(const Key *found_key, Node *node, Node *successor_node) {
            int found_key_index = -1;
            if(found_key != nullptr) {
                found_key_index = node->remove_key(found_key);
            }
            int old_keys_count = successor_node->keys_count();
            std::unique_ptr<Key> successor_min_key = std::move(successor_node->keys[0]);
            successor_node->move_keys_to_front(old_keys_count);
            int parent_max_key_index = node->parent->keys_count() - 1;
            std::unique_ptr<Key> parent_max_key = std::move(node->parent->keys[parent_max_key_index]);
            node->parent->keys[parent_max_key_index] = std::move(successor_min_key);
            if(found_key_index > -1) {
                node->keys[found_key_index] = std::move(parent_max_key);
                node->sort_keys();
            } else {
                node->keys[node->keys_count()] = std::move(parent_max_key);
            }
            if(found_key != nullptr && !successor_node->is_leaf() && !node->is_leaf()) {
                node->children.push_back(std::move(std::unique_ptr<Node>(successor_node->get_predecessor_child_node(found_key))));
            }
        }

        Node *Node::get_predecessor_node() {
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

        Node *Node::get_successor_node() {
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

        Node *Node::get_predecessor_child_node(const Key *key) {
            Node *predecessor_child_node = nullptr;
            for(int i = 0; i < children.size(); i++) {
                if(predecessor_child_node != nullptr) {
                    break;
                }
                for(int j = 0; j < children.at(i)->keys_count(); j++) {
                    if(children.at(i)->keys[j]->hash >= key->hash) {
                        if(children.at(i)->keys_count() > 1 && j > 0) {
                            predecessor_child_node = children.at(i).get();
                            break;
                        }
                        if(i - 1 >= 0) {
                            predecessor_child_node = children.at(i - 1).get();
                        } else {
                            predecessor_child_node = children.at(i).get();
                        }
                        break;
                    }
                }
            }
            return predecessor_child_node;
        }

        Node *Node::get_successor_child_node(const Key *key) {
            Node *successor_child_node = nullptr;
            for(const auto &i : children) {
                if(i->keys[0]->hash >= key->hash/* && i - 1 >= 0*/) {
                    successor_child_node = i.get();
                    break;
                }
            }
            return successor_child_node;
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
            return index;
        }

        void Node::move_keys_to_front(int old_count) {
            for(int i = 0; i < old_count; i++) {
                if(keys[i] != nullptr && i - 1 >= 0 && keys[i - 1] == nullptr) {
                    keys[i - 1] = std::move(keys[i]);
                }
            }
        }

        int Node::get_index_in_children() {
            int index = -1;
            if(parent != nullptr) {
                for(int i = 0; i < parent->children.size(); i++) {
                    if(parent->children.at(i).get() == this) {
                        index = i;
                        break;
                    }
                }
            }
            return index;
        }

        void Node::found_keys_count(Key *key, std::vector<kvdb::btree::Key *> *keys) {
            if(key == nullptr || key->deleted) {
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
            while(i < BTREE_MAX_DEGREE) {
                int j = i;
                while(j > 0 && keys[j - 1] != nullptr && keys[j] != nullptr && keys[j - 1]->hash > keys[j]->hash) {
                    std::swap(keys[j], keys[j - 1]);
                    j--;
                }
                i++;
            }
        }

        void Node::add_child_node(std::unique_ptr<Node> node, int index) {
            if(node->parent == nullptr || node->parent != this) {
                node->parent = this;
            }
            bool exists = false;
            for(const auto &i : children) {
                if(i.get() == node.get()) {
                    exists = true;
                    break;
                }
            }
            if(!exists) {
                if(index == -1) {
                    children.push_back(std::move(node));
                } else {
                    children.insert(children.begin() + index, std::move(node));
                }
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

        bool Node::has_min_keys() const {
            return keys_count() >= (int)std::ceil((BTREE_MAX_DEGREE - 1) / 2);
        }

        bool Node::is_root() {
            return !is_leaf() && parent == nullptr;
        }


    } // namespace btree

} // namespace kvdb


