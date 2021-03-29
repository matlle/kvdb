/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "BTree.h"
#include "../utils/log.hpp"

namespace btree {

    BTree::BTree() {
        //root = std::make_shared<Node>();
        root = new Node();
    }

    /*std::unique_ptr<BTree> &BTree::get_instance() {
        if(instance == nullptr) {
            instance = std::unique_ptr<BTree>(new BTree);
        }
        return instance;
    }*/

    Node *BTree::find_root(Node *node, Node *parent) {
        if(parent == nullptr) {
            return node;
        }
        return find_root(node->parent, node->parent->parent);
    }

    BTree::~BTree() {
    }

}