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

        bool Key::serialize(Stream *stream) {
            if(stream == nullptr || !stream->opened()) {
                return false;
            }
            uint32_t bytes_written = stream->write_ulong(hash);
            if(bytes_written == 0) {
                return false;
            }
            uint32_t stream_pos = stream->total_bytes - bytes_written;
            return stream->write_uint(value->stream_data_pos) > 0
                   && stream->write_byte(!deleted ? 0 : 1) > 0
                   && stream->write_uint(stream_pos) > 0;
        }

        bool Key::serialize_deleted(Stream *stream) {
            if(stream == nullptr || !stream->opened()) {
                return false;
            }
            if(!stream->seek(value->stream_tree_pos)) {
                return false;
            }
            return stream->write_string(std::string("00000000000000000"), false) == 17 && stream->seek_end();
        }

    } // namespace btree

} // namespace kvdb

