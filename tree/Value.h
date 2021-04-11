/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <string>

namespace kvdb {

    namespace btree {

        class Value {
        public:
            //std::string data = std::string();
            uint32_t stream_tree_pos = 0;
            uint32_t stream_data_pos = 0;

            Value();
            //explicit Value(const std::string &value);
        };

    } // namespace btree

} // namespace kvdb


