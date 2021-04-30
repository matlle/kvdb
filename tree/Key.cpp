/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "Key.h"
#include "Node.h"

namespace kvdb {

    namespace btree {

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
            uint32_t bytes_written = stream_tree->write_ulong(hash);
            if(bytes_written == 0) {
                return false;
            }
            //value->stream_tree_pos = stream_tree->total_bytes - bytes_written;
            //stream_tree->total_bytes - bytes_written;
            return stream_tree->write_uint(stream_data_pos) > 0;
                //&& stream_tree->write_byte(!deleted ? 0 : 1) > 0;
        }

        bool Key::serialize_deleted(Stream *stream_tree) {
            if(stream_tree == nullptr || !stream_tree->opened()) {
                return false;
            }
            if(!stream_tree->seek(value->stream_tree_pos)) {
                return false;
            }
            return stream_tree->write_string(std::string("0000000000000"), false) == 13 && stream_tree->seek_end();
        }

    } // namespace btree

} // namespace kvdb

