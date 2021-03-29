/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <iostream>
#include <memory>
#include "utils/log.hpp"
#include "tree/BTree.h"
#include "utils/random.hpp"
#include "cli/Cli.h"

int main(int argc, char *argv[]) {
    //std::unique_ptr<btree::BTree> &btree = btree::BTree::get_instance();
    /*std::unique_ptr<btree::BTree> btree = std::make_unique<btree::BTree>();
    btree->root = btree->root->insert_key(1, "val_1");
    btree->root = btree->root->insert_key(2, "val_2");
    btree->root = btree->root->insert_key(3, "val_3");
    btree->root = btree->root->insert_key(4, "val_4");
    btree->root = btree->root->insert_key(5, "val_5");
    btree->root = btree->root->insert_key(6, "val_6");
    btree->root = btree->root->insert_key(7, "val_7");
    btree->root = btree->root->insert_key(8, "val_8");
    btree->root = btree->root->insert_key(9, "val_9");
    btree->root = btree->root->insert_key(10, "val_10");
    btree->root = btree->root->insert_key(11, "val_11");
    btree->root = btree->root->insert_key(12, "val_12");
    btree->root = btree->root->insert_key(13, "val_13");
    btree->root = btree->root->insert_key(14, "val_14");
    btree->root = btree->root->insert_key(15, "val_15");
    btree->root = btree->root->insert_key(16, "val_16");
    btree->root = btree->root->insert_key(17, "val_17");
    btree->root = btree->root->insert_key(18, "val_18");
    btree->root = btree->root->insert_key(19, "val_19");
    btree->root = btree->root->insert_key(20, "val_20");
    btree->root = btree->root->insert_key(21, "val_21");
    btree->root = btree->root->insert_key(22, "val_22");
    btree->root = btree->root->insert_key(23, "val_23");
    btree->root = btree->root->insert_key(24, "val_24");
    btree->root = btree->root->insert_key(25, "val_25");
    btree->root = btree->root->insert_key(26, "val_26");
    btree->root = btree->root->insert_key(27, "val_27");
    btree->root = btree->root->insert_key(28, "val_28");
    btree->root = btree->root->insert_key(29, "val_29");
    btree->root = btree->root->insert_key(30, "val_30");*/

    kvdb::Cli cli;
    cli.prompt();

    return 0;
}
