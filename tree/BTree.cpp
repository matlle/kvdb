/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "BTree.h"
#include "../utils/log.hpp"

namespace kvdb {

    namespace tree {

        BTree::BTree() : plus(false), root(new Node) {
        }

    } // namespace tree

} // namespace kvdb

