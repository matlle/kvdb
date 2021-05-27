/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>

namespace kvdb {

    namespace tree {

        class Tree {
        public:
            static constexpr int8_t BTREE = 0;
            static constexpr int8_t BPTREE = 1;
            int8_t type = BTREE;

            explicit Tree(int8_t type);
        };

    } // namespace tree

} // namespace kvdb
