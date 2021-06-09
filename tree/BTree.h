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

    namespace tree {

        class BTree {
        public:
            Node *root;
            bool plus;

            BTree();
            explicit BTree(bool plus);
            ~BTree();
            static Node *find_root_node(Node *node, Node *parent);
            static std::unique_ptr<BTree> deserialize(Stream *stream_tree);
            bool is_bptree() const;
            void update_node_links() const;
            void set_root_node(Node *node);
        };

    } // namespace btree

} // namespace kvdb

