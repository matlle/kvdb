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
            Node *parent = nullptr;
            std::unique_ptr<Key> keys[BTREE_MAX_DEGREE]{};
            std::vector<std::unique_ptr<Node>> children{};

            explicit Node();
            explicit Node(const std::string &key, const std::string &value);
            static size_t hash_key(const std::string &key);
            bool is_leaf();
            static void search_key(Node *&node, const Key *key, Key *&found_key);
            static Node *delete_key(Node *&found_node, Key *key, uint32_t *count_keys_deleted, Stream *stream);
            int remove_key(const Key *key);
            void move_keys_to_front(int old_count);
            Node *get_predecessor_node();
            Node *get_successor_node();
            Node *get_predecessor_child_node(const Key *key);
            Node *get_successor_child_node(const Key *key);
            int get_index_in_children();
            static void predecessor_max_key_swap_with_parent_max_key(const Key *found_key, Node *predecessor_node, Node *node);
            static void successor_min_key_swap_with_parent_max_key(const Key *found_key, Node *found_node, Node *successor_node);
            static void merge_node_to_predecessor(const Key *found_key, Node *predecessor_node, Node *node);
            static void merge_node_to_successor(const Key *found_key, Node *node, Node *successor_node);
            static Node *remove_key_in_leaf_node_with_min_keys(const Key *found_key, Node *node);
            bool has_more_keys() const;
            bool has_min_keys() const;
            bool is_root();
            static void check_parent_with_min_keys(Node *parent_node);
            Node *insert_key_to_leaf(std::unique_ptr<Key> key);
            Node *insert_key(std::unique_ptr<Key> key);
            void add_to_keys(std::unique_ptr<Key> key);
            void sort_keys();
            Node *split();
            std::unique_ptr<Node> split_keys();
            void add_child_node(std::unique_ptr<Node> node, int index=-1);
            void move_half_children_to_node(Node *node);
            void merge_node(Node *node, bool merging_to_predecessor=true);
            int find_merging_node_key_index(Node *node, bool merging_to_predecessor=true);
            [[nodiscard]] int keys_count() const;
            static Node *find_child_node(const Key *key, const Node *node);
            int binary_search(const Key *key);
            int contains_key(const Key *key, int *found_key_index);
            static void found_keys_count(Key *key, std::vector<kvdb::btree::Key *> *keys);
            static void insert_into_found_keys(Key *key, std::vector<kvdb::btree::Key *> *found_keys);
        };

    } // namespace btree

} // namespace kvdb


