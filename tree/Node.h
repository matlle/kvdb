/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <memory>
#include <cmath>
#include <vector>
#include "../utils/definies.h"
#include "Key.h"

namespace kvdb {

    namespace btree {

        class Node {
        public:
            //std::shared_ptr<Node> parent = nullptr;
            Node *parent = nullptr;
            std::unique_ptr<Key> keys[BTREE_MAX_DEGREE - 1]{};
            std::vector<std::unique_ptr<Node>> children{};
            //std::unique_ptr<Node> children[BTREE_MAX_DEGREE]{};

            explicit Node();
            explicit Node(const std::string &key, const std::string &value);
            explicit Node(const std::size_t &hash, const std::string &value);
            static size_t hash_key(const std::string &key);
            bool is_leaf();
            static void find_key(Node *node, Key *search_key, std::vector<kvdb::btree::Key *> *found_keys, bool searched=false);
            static Key *find_key_in_children(Node *&node, const Key *search_key);
            Node *insert_key_to_leaf(std::unique_ptr<Key> key);
            Node *insert_key(std::unique_ptr<Key> key);
            void add_to_keys(std::unique_ptr<Key> key);
            void sort_keys();
            Node *split(std::unique_ptr<Key> key);
            std::unique_ptr<Node> split_keys(std::unique_ptr<Key> key);
            void add_child_node(std::unique_ptr<Node> node);
            void shift_children_to_left();
            [[nodiscard]] int keys_count() const;
            static Node *find_child_node(const Key *key, const Node *node);
            int binary_search(const Key *key);
            int contains_key(const Key *key, int *key_found_index);
        };

    }

} // namespace kvdb


