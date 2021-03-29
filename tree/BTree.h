/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <memory>
#include "../utils/definies.h"
#include "Node.h"

namespace kvdb::btree {

    class BTree {
    private:
    public:
        Node *root = nullptr;

        BTree();
        ~BTree();
        static Node *find_root(Node *node, Node *parent);
    };

}


