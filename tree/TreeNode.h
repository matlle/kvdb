/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>
#include <cmath>
#include "../db/Stream.h"
#include "../utils/definies.h"
#include "TreeKey.h"

namespace kvdb {

    namespace tree {

        class TreeNode {
        public:
            static constexpr uint16_t ROOT_ID = 0;
            static uint16_t NEXT_NODE_ID;
            uint16_t id = 0;
            std::string table_path = std::string();
            std::string keys_stream_path = std::string();
            std::string children_stream_path = std::string();
            std::unique_ptr<Stream> keys_stream = nullptr;
            std::unique_ptr<Stream> children_stream = nullptr;
            std::shared_ptr<TreeKey> keys[PAGE_SIZE]{};
            std::vector<std::unique_ptr<TreeNode>> children{};

            explicit TreeNode(const std::string &table_path);
            TreeNode(uint16_t id, const std::string &table_path);
            static std::shared_ptr<TreeNode> disk_read(uint16_t node_id, const std::string &table_path);
            bool disk_read_keys();
            bool disk_read_children_node(bool lazy_read=true);
            bool disk_write(bool keys_was_moved=false, bool children_was_moved=false);
            bool keys_write();
            bool children_write();
            bool is_full() const;
            bool is_leaf() const;
            int keys_count() const;
            void sort_keys();
            void insert_into_keys(std::shared_ptr<TreeKey> key);
            void move_keys_to_front();
            bool delete_keys_stream();
            bool delete_children_stream();
            bool keys_stream_set_mode(const char *mode);
            bool children_stream_set_mode(const char *mode);
            static kvdb::StatusEx insert(TreeNode *node, std::shared_ptr<tree::TreeKey> treeKey, bool moving_from_child_to_parent=false);
            static kvdb::StatusEx insert_not_full(TreeNode *node, std::shared_ptr<tree::TreeKey> treeKey, bool moving_from_child_to_parent=false);
            static void find_child_node(TreeNode *node, const TreeKey *treeKey, uint16_t *node_id);
        };

    } // tree

} // kvdb