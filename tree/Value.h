/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <string>

namespace kvdb::btree {

    class Value {
    public:
        explicit Value(const std::string &value);

        std::string data = std::string();
    };

} // namespace kvdb


