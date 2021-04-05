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
            if(parent == nullptr) {
                return node;
            }
            return find_root_node(node->parent, node->parent->parent);
        }

        std::unique_ptr<BTree> BTree::deserialize(const Stream *stream) {
            if(stream == nullptr) {
                return nullptr;
            }
            std::unique_ptr<BTree> tree = std::make_unique<BTree>();
            size_t hash = 0;
            while((hash = stream->read_ulong()) > 0) {
                std::unique_ptr<Key> key = std::make_unique<Key>();
                key->hash = hash;
                key->value->file_pos = stream->read_uint();
                key->deleted = stream->read_byte() != 0;
                if(key->deleted) {
                    //
                } else {
                    tree->root = tree->root->insert_key_to_leaf(std::move(key));
                }
            }
            return tree;
        }

        BTree::~BTree() = default;

    } // namespace btree

} // namespace kvdb

