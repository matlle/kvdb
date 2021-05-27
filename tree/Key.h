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
            uint16_t hash = 0;
            std::shared_ptr<Value> value = std::make_shared<Value>();;
            std::vector<std::unique_ptr<Key>> siblings{};
            std::vector<std::unique_ptr<Key>> twins{};
            bool deleted = false;
            uint32_t stream_data_pos = 0;

            Key();
            Key(const std::string &key, const std::string &value);
            explicit Key(const std::string &key);

            bool serialize(Stream *stream_tree) const;
        };

    }

} // namespace kvdb
