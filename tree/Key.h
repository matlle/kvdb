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

    namespace tree {

        class Key {
        public:
            uint16_t hash = 0;
            std::shared_ptr<Value> value = std::make_shared<Value>();;
            std::vector<std::shared_ptr<Key>> twins{};
            bool deleted = false;
            uint32_t stream_data_pos = 0;

            Key();
            Key(const std::string &key, const std::string &value);
            explicit Key(uint16_t key);
            explicit Key(const std::string &key);

            bool serialize(Stream *stream_tree) const;
            std::shared_ptr<Key> copy();
        };

    } // namespace btree

} // namespace kvdb
