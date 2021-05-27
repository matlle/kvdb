/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "BTree.h"
#include "../utils/log.hpp"

namespace kvdb {

    namespace btree {

        BTree::BTree() {
            root = new Node();
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

        BTree::~BTree() = default;

    } // namespace btree

} // namespace kvdb

