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
            static void find_key(Node *node, const Key *key, std::vector<kvdb::btree::Key *> *found_keys, bool searched=false);
            static void search_key(Node *&node, const Key *key, Key *&found_key, bool searched= false);
            static Key *search_key_in_children(Node *&node, const Key *search_key);
            static void delete_key(Node *node, Key *key, std::vector<kvdb::btree::Key *> *found_keys);
            static Node *delete_key(Node *&node, Key *key, uint32_t *count_keys_deleted, Stream *stream);
            int remove_key(const Key *key);
            void move_key_at_index(int index, int old_count);
            Node *predecessor_node();
            Node *successor_node();
            bool has_more_keys() const;
            Node *insert_key_to_leaf(std::unique_ptr<Key> key);
            Node *insert_key(std::unique_ptr<Key> key);
            void add_to_keys(std::unique_ptr<Key> key);
            void sort_keys();
            Node *split(std::unique_ptr<Key> key);
            std::unique_ptr<Node> split_keys(std::unique_ptr<Key> key);
            void add_child_node(std::unique_ptr<Node> node);
            void shift_children_to_left();
            void move_half_children_to_node(Node *node);
            void merge_node(Node *node);
            int find_merging_node_key_index(Node *node, bool merging_to_predecessor= true);
            [[nodiscard]] int keys_count() const;
            static Node *find_child_node(const Key *key, const Node *node);
            int binary_search(const Key *key);
            int contains_key(const Key *key, int *found_key_index);
            static void found_keys_count(Key *key, std::vector<kvdb::btree::Key *> *keys);
            static void insert_into_found_keys(Key *key, std::vector<kvdb::btree::Key *> *found_keys);
        };

    }

} // namespace kvdb


