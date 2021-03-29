/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "Key.h"
#include "Node.h"

namespace btree {

    Key::Key(const std::string &key, const std::string &value) {
        this->key = key;
        this->hash = Node::hash_key(key);
        this->value = std::make_shared<Value>(value);
    }

    Key::Key(const size_t &hash, const std::string &value) {
        this->hash = hash;
        this->value = std::make_shared<Value>(value);
    }

}
