/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <string>

namespace kvdb {

    namespace tree {

        class TreeKey {
        public:
            std::string key;
            std::string value;
            uint16_t key_hash = 0;
            uint16_t value_hash = 0;
            bool key_serialized = false;
            bool value_serialized = false;

            explicit TreeKey();
            TreeKey(const std::string &key, const std::string &value);
            static uint16_t hash16(const std::string &key);

        };

    } // tree

} // kvdb
