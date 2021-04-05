/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <memory>
#include "../utils/definies.h"
#include "Node.h"
#include "../db/Stream.h"

namespace kvdb {

    namespace btree {

        class BTree {
        private:
        public:
            Node *root = nullptr;

            BTree();
            ~BTree();
            static Node *find_root_node(Node *node, Node *parent);
            static std::unique_ptr<BTree> deserialize(const Stream *stream);
        };

    }

}


