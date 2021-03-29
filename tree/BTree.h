/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <memory>
#include "../utils/definies.h"
#include "Node.h"

namespace btree {

    class BTree {
    private:
        //inline static std::unique_ptr<BTree> instance = nullptr;
    public:
        uint32_t degree = BTREE_MAX_DEGREE;
        Node *root = nullptr;

        BTree();
        ~BTree();
        //static std::unique_ptr<BTree> &get_instance();
        static Node *find_root(Node *node, Node *parent);
    };

}

