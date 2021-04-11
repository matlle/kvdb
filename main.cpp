/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "tree/BTree.h"
#include "cli/Cli.h"

int main(int argc, char *argv[]) {
    /*std::unique_ptr<kvdb::btree::BTree> btree = std::make_unique<kvdb::btree::BTree>();
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
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(32, "val_32"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(33, "val_33"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(34, "val_34"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(35, "val_35"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(36, "val_36"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(37, "val_37"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(38, "val_38"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(39, "val_39"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(40, "val_40"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(41, "val_41"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(42, "val_42"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(43, "val_43"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(44, "val_44"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(45, "val_45"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(46, "val_46"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(47, "val_47"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(48, "val_48"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(49, "val_49"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(50, "val_50"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(51, "val_51"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(52, "val_52"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(53, "val_53"));
    btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(54, "val_54"));
    //btree->root = btree->root->insert_key_to_leaf(std::make_unique<kvdb::btree::Key>(6, "val_6"));

    kvdb::btree::Key k(6, "val_6");
    std::vector<kvdb::btree::Key *> keys_found{};
    kvdb::btree::Node::find_key(btree->root, &k, &keys_found);

    kvdb::btree::Key k(5, "val_5");
    std::vector<kvdb::btree::Key *> keys_found{};
    kvdb::btree::Node::find_key(btree->root, &k, &keys_found);*/


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

    kvdb::btree::Key k(1, "val_1");
    uint32_t count = 0;
    btree->root = kvdb::btree::Node::delete_key(btree->root, &k, &count, nullptr);

    kvdb::Cli cli;
    cli.prompt();
    return 0;
}
