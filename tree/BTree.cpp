/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "BTree.h"
#include "../utils/log.hpp"

namespace kvdb::btree {

    BTree::BTree() {
        root = new Node();
    }

    Node *BTree::find_root(Node *node, Node *parent) {
        if(parent == nullptr) {
            return node;
        }
        return find_root(node->parent, node->parent->parent);
    }

    BTree::~BTree() = default;

} // namespace kvdb

