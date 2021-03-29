/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <memory>
#include <cmath>
#include "../utils/definies.h"
#include "Key.h"

namespace kvdb::btree {

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
        Node *insert_key(const std::size_t &hash, const std::string &value);
        Node *insert_key(std::unique_ptr<Key> key);
        std::unique_ptr<Key> add_to_keys(std::unique_ptr<Key> key);
        void sort_keys();
        Node *split(std::unique_ptr<Key> key);
        std::unique_ptr<Node> split_keys(std::unique_ptr<Key> key);
        void add_child_node(std::unique_ptr<Node> node);
        void shift_children_to_left();
    };

} // namespace kvdb


