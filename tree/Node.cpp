/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "Node.h"
#include "BTree.h"
#include "../utils/log.hpp"

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
        for(auto & i : children) {
            if(i != nullptr) {
                ret = false;
                break;
            }
        }
        return ret;
    }

    std::unique_ptr<Key> Node::add_to_keys(std::unique_ptr<Key> key) {
        for(auto & i : keys) {
            if(i == nullptr) {
                i = std::move(key);
                break;
            }
        }
        return key;
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
            for(int j = 0; j < children.size() / 2; j++) {
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

    Node *Node::insert_key(const size_t &hash, const std::string &value) {
        std::unique_ptr<Key> key = std::make_unique<Key>(hash, value);
        if(is_leaf()) {
            Node *node = insert_key(std::move(key));
            return BTree::find_root(node, node->parent);
        }
        Node *node = this;
        do {
            ssize_t index = -1;
            for(size_t i = 0; i < node->children.size() - 1; i++) {
                if(key->hash < node->keys[i]->hash) {
                    index = -1;
                    break;
                }
                if(key->hash == node->keys[i]->hash) {
                    index = i + 1;
                }
            }
            if(index == -1) {
                index = node->children.size() - 1;
            }
            node = node->children[index].get();
        } while (!node->is_leaf());
        Node *node1 = node->insert_key(std::move(key));
        return BTree::find_root(node1,  node1->parent);
        //return this;
    }

    Node *Node::insert_key(std::unique_ptr<Key> key) {
        std::unique_ptr<Key> k = add_to_keys(std::move(key));
        if(k != nullptr) { // node is full
            //split(std::move(k));
            return split(std::move(k));
        }
        sort_keys();
        return this;
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

}
