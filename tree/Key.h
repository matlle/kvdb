/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include "Value.h"

namespace btree {

    class Key {
    public:
        Key(const std::string &key, const std::string &value);
        Key(const std::size_t &hash, const std::string &value);

        std::string key = std::string();
        std::size_t hash = 0;
        std::shared_ptr<Value> value = nullptr;
    };

}

