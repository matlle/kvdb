/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "Key.h"
#include "Node.h"

namespace kvdb {

    namespace btree {

        Key::Key() = default;

        Key::Key(const std::string &key, const std::string &value) {
            //this->key = key;
            this->hash = Node::hash_key(key + value);
            //this->value = std::make_shared<Value>(value);
        }

        Key::Key(const size_t &hash, const std::string &value) {
            this->hash = hash;
            //this->value = std::make_shared<Value>(value);
        }

        bool Key::serialize(Stream *stream) {
            if(stream == nullptr || !stream->opened()) {
                return false;
            }
            return stream->write_ulong(hash) > 0
                   && stream->write_uint(value->file_pos) > 0
                   && stream->write_byte(!deleted ? 0 : 1) > 0;
        }

    } // namespace btree

} // namespace kvdb

