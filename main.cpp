/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
//#include "cli/Cli.h"
#include "db/Database.h"
#include "utils/log.hpp"

void students_put(kvdb::Table *table, uint32_t number_of_records=100000) {
    for(uint32_t i = 0; i < number_of_records; i++) {
        kvdb::StatusEx status = table->put(std::to_string(i), "Value" + std::to_string(i));
        if(status.is_error()) {
            PRINT_ERROR(status.msg, nullptr);
            break;
        }
    }
}

int main() {
    //kvdb::Cli cli;
    //cli.prompt();

    std::unique_ptr<kvdb::Database> db = std::make_unique<kvdb::Database>("/home/matlle/testdb");
    if(!db->open()) {
        if(db->name.empty()) {
            PRINT_ERROR("failed to open database", nullptr);
        } else {
            PRINT_ERROR("failed to open database '%'", db->name.c_str());
        }
        return EXIT_FAILURE;
    }
    kvdb::Table *table = db->get_table("students");
    //students_put(table, 10);

    /*std::string value = std::string();
    kvdb::StatusEx status = table->get("2", &value);
    if(status.is_error()) {
        PRINT_ERROR(status.msg, nullptr);
    } else {
        if(value.empty()) {
            PRINT("value not found", nullptr);
        } else {
            PRINT(value.c_str(), nullptr);
        }
    }*/

    kvdb::StatusEx status;
    for(uint32_t i = 0; i < 10; i++) {
        status = table->remove(std::to_string(i));
        if(status.is_error()) {
            PRINT_ERROR("%s %s", status.msg, std::to_string(i).c_str());
        } else {
            PRINT("%s: key %s deleted", status.msg, i);
        }
    }


    //students_put(table);
    //std::string value;
    //table->get("3", &value);


    return EXIT_SUCCESS;
}