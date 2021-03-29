/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "Value.h"

namespace kvdb::btree {

    Value::Value(const std::string &value) {
        this->data = value;
    }

} // namespace kvdb
