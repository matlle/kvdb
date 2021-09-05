/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "BTree.h"
#include "../utils/log.hpp"

namespace kvdb {

    namespace tree {

        BTree::BTree() : plus(false), root(new Node) {
            root->tree = this;
        }

        BTree::BTree(bool plus) : plus(plus), root(new Node) {
            root->tree = this;
        }

        Node *BTree::find_root_node(Node *node, Node *parent) {
            if (parent == nullptr) {
                return node;
            }
            return find_root_node(node->parent, node->parent->parent);
        }

        std::unique_ptr<BTree> BTree::deserialize(Stream *stream_tree) {
            if (stream_tree == nullptr) {
                return nullptr;
            }
            uint32_t slen = 0;
            std::string row_id = std::string();
            slen = stream_tree->read_uint();
            row_id = stream_tree->read_string(slen);
            std::unique_ptr<BTree> tree = std::make_unique<BTree>();
            uint16_t hash = 0;
            while ((hash = stream_tree->read_ushort()) > 0) {
                std::unique_ptr<Key> key = std::make_unique<Key>();
                key->hash = hash;
                key->stream_data_pos = stream_tree->read_uint();
                tree->root = tree->root->insert_key_to_leaf(std::move(key));
            }
            return tree;
        }

        bool BTree::is_bptree() const {
            return plus;
        }

        void BTree::update_node_links() const {
            if(root->is_leaf()) {
                return;
            }
            Node *child_node = nullptr;
            do {
                child_node = child_node == nullptr ? root->children.at(0).get() : child_node->children.at(0).get();
            } while(!child_node->is_leaf());

            Node *parent_node = child_node->parent;
            size_t i;
            for(i = 0; i < parent_node->children.size(); i++) {
                if(i + 1 < parent_node->children.size()) {
                    parent_node->children.at(i)->next = parent_node->children.at(i + 1).get();
                } else {
                    parent_node->children.at(i)->next = nullptr;
                }
            }

            Node *parent_node1 = parent_node->parent;
            if(parent_node1 != nullptr) {
                parent_node->children.at(parent_node->children.size() - 1)->next = parent_node1->children.at(1)->children.at(0).get();
                for(i = 1; i < parent_node1->children.size(); i++) {
                    child_node = parent_node1->children.at(i).get();
                    for(size_t j = 0; j < child_node->children.size(); j++) {
                        if(j + 1 < child_node->children.size()) {
                            child_node->children.at(i)->next = child_node->children.at(j + 1).get();
                        } else {
                            child_node->children.at(i)->next = nullptr;
                        }
                    }
                }
            }
        }

        void BTree::set_root_node(Node *node) {
            root = node;
            if(root->tree->is_bptree()) {
                root->tree->update_node_links();
            }
        }

        BTree::~BTree() = default;

    } // namespace tree

} // namespace kvdb

