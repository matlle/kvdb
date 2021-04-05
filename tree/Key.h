/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include "Value.h"
#include "../db/Stream.h"

namespace kvdb {

    namespace btree {

        class Key {
        public:
            //std::string key = std::string();
            std::size_t hash = 0;
            std::shared_ptr<Value> value = std::make_shared<Value>();;
            std::vector<std::unique_ptr<Key>> twins{};
            bool deleted = false;

            Key();
            Key(const std::string &key, const std::string &value);
            Key(const std::size_t &hash, const std::string &value);

            bool serialize(Stream *stream);
        };

    }

} // namespace kvdb
