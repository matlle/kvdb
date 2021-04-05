/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "tree/BTree.h"
#include "cli/Cli.h"

int main(int argc, char *argv[]) {
    std::unique_ptr<kvdb::btree::BTree> btree = std::make_unique<kvdb::btree::BTree>();
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(1, "val_1"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(2, "val_2"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(3, "val_3"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(4, "val_4"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(5, "val_5"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(6, "val_6"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(7, "val_7"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(8, "val_8"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(9, "val_9"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(10, "val_10"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(11, "val_11"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(12, "val_12"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(13, "val_13"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(14, "val_14"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(15, "val_15"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(16, "val_16"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(17, "val_17"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(18, "val_18"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(19, "val_19"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(20, "val_20"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(21, "val_21"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(22, "val_22"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(23, "val_23"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(24, "val_24"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(25, "val_25"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(26, "val_26"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(27, "val_27"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(28, "val_28"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(29, "val_29"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(30, "val_30"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(31, "val_31"));
    //btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(6, "val_6"));

    /*kvdb::btree::Key k(6, "val_6");
    std::vector<kvdb::btree::Key *> keys_found{};
    kvdb::btree::Node::find_key(btree->root, &k, &keys_found);

    kvdb::btree::Key k(5, "val_5");
    std::vector<kvdb::btree::Key *> keys_found{};
    kvdb::btree::Node::find_key(btree->root, &k, &keys_found);*/


    kvdb::Cli cli;
    cli.prompt();
    return 0;
}
