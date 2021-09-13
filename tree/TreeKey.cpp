/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "TreeKey.h"

namespace kvdb {

    namespace tree {

        TreeKey::TreeKey() = default;

        TreeKey::TreeKey(const std::string &key, const std::string &value) {
            this->key = key;
            this->value = value;
            this->key_hash = hash16(this->key);
            if(!value.empty()) {
                this->value_hash = hash16(this->value);
            }
        }

        uint16_t TreeKey::hash16(const std::string &value) {
            return std::hash<std::string>{}(value);
        }

    }
}