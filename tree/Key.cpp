/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "Key.h"
#include "Node.h"

namespace kvdb {

    namespace tree {

        Key::Key() = default;

        Key::Key(const std::string &key, const std::string &value) {
            this->hash = (uint16_t)Node::hash_key(key + value);
        }

        Key::Key(const std::string &key) {
            this->hash = (uint16_t)Node::hash_key(key);
        }

        bool Key::serialize(Stream *stream_tree) const {
            if(stream_tree == nullptr || !stream_tree->opened()) {
                return false;
            }
            uint32_t bytes_written = stream_tree->write_ushort(hash);
            if(bytes_written == 0) {
                return false;
            }
            return stream_tree->write_uint(stream_data_pos) > 0;
        }

        Key::Key(uint16_t key) {
            this->hash = key;
        }

        std::shared_ptr<Key> Key::copy() {
            return std::make_shared<Key>(*this);
        }

    } // namespace tree

} // namespace kvdb

