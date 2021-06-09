/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <string>

namespace kvdb {

    namespace tree {

        class Value {
        public:
            uint32_t stream_tree_pos = 0;
            uint32_t stream_data_pos = 0;

            Value();
        };

    } // namespace btree

} // namespace kvdb


