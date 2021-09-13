/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
//#include "cli/Cli.h"
#include "db/Database.h"
#include "utils/log.hpp"

void students_put(kvdb::Table *table) {
    for(uint32_t i = 0; i < 1000000; i++) {
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
    /*kvdb::StatusEx status = table->put("1", "Value1");
    status = table->put("2", "Value2");
    status = table->put("3", "Value3");
    status = table->put("4", "Value4");
    status = table->put("5", "Value5");
    status = table->put("6", "Value6");
    status = table->put("7", "Value7");
    status = table->put("8", "Value8");
    status = table->put("9", "Value9");
    status = table->put("10", "Value10");
    status = table->put("11", "Value11");
    status = table->put("12", "Value12");
    status = table->put("13", "Value13");
    status = table->put("14", "Value14");
    status = table->put("15", "Value15");
    status = table->put("16", "Value16");
    status = table->put("17", "Value17");
    status = table->put("18", "Value18");
    status = table->put("19", "Value19");
    status = table->put("20", "Value20");
    if(status.is_error()) {
        PRINT_ERROR(status.msg, nullptr);
    }*/

    std::string value = std::string();
    kvdb::StatusEx status = table->get("11", &value);
    if(status.is_error()) {
        PRINT_ERROR(status.msg, nullptr);
    } else {
        PRINT(value.c_str(), nullptr);
    }
    //students_put(table);
    //std::string value;
    //table->get("3", &value);


    return EXIT_SUCCESS;
}