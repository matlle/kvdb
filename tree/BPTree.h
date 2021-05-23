/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include "Tree.h"

namespace kvdb {

    namespace bptree {

        class BPTree : tree::Tree {
        public:
            explicit BPTree(int8_t type=tree::Tree::BPTREE);
        };

    } // namespace bptree

} // namespace kvdb
