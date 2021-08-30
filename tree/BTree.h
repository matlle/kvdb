/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <memory>
#include "../utils/definies.h"
#include "Node.h"

namespace kvdb {

    namespace tree {

        class BTree {
        public:
            Node *root;
            bool plus;

            BTree();
        };

    } // namespace btree

} // namespace kvdb

