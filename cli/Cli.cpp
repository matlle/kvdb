/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <iostream>
#include <sstream>
#include <fstream>
#include "Cli.h"
#include "../utils/log.hpp"

#define CHECK_DB_OPENED() \
if(db == nullptr || !db->opened) { \
    error_str = "No database selected. Please select a database first.\nTry: selectdb /path/your_db"; \
    status = Status::ERROR_; \
    goto END_QUERY; \
}

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::chrono::duration;

namespace kvdb {

    Cli::Cli() {
        PRINT("Kvdb v.%s", VERSION);
    }

    void Cli::prompt() {
        std::cout << (db != nullptr && db->opened ? db->name : "") << "> ";
        getline(std::cin, command);
        std::vector<std::string> words = split_string(command, ' ');
        std::string error_str = std::string();
        auto t1 = high_resolution_clock::now();
        kvdb::Status status = kvdb::Status::ERROR_;
        int8_t cmd = get_command(words, command);
        if(cmd == -1) {
            print_help();
            return;
        }
        if(cmd == EXIT) {
            exit(EXIT_SUCCESS);
        }
        if(cmd == HELP) {
            print_help();
            return;
        }
        if(cmd == SELECTDB) {
            if(db != nullptr) {
                db->close();
                db.reset();
            }
            db = std::make_unique<Database>(words.at(1));
            if(!db->open()) {
                if(db->name.empty()) {
                    PRINT_ERROR("failed to open database", nullptr);
                } else {
                    PRINT_ERROR("failed to open database '%'", db->name.c_str());
                }
                prompt();
                return;
            }
            status = kvdb::Status::OK_;
            PRINT("database '%s' selected", db->name.c_str());
        } else if(cmd == QUERYLOAD) {
            CHECK_DB_OPENED();
            if(!Stream::file_exists(words.at(1).c_str())) {
                error_str = "failed to open query file " + words.at(1);
                status = Status::ERROR_;
                goto END_QUERY;
            }
            std::ifstream file(words.at(1).c_str());
            std::string line = std::string();
            while(std::getline(file, line)) {
                if(line.empty()) {
                    continue;
                }
                std::unique_ptr<TableQuery> table_action = TableQuery::get_table_query(line);
                Table *table = nullptr;
                if(table_action == nullptr || (table = db->get_table(table_action->table_name.c_str())) == nullptr) {
                    continue;
                }
                status = table->process_query(std::move(table_action));
                if(status == Status::ERROR_) {
                    error_str = "failed to process query\n" + line + "\nfinishing loading data...";
                    break;
                }
                PRINT("process query: %s", line.c_str());
            }
        } else if(is_table_op(cmd)) {
            CHECK_DB_OPENED();
            std::unique_ptr<TableQuery> table_query = TableQuery::get_table_query(command);
            if(table_query != nullptr) {
                Table *table = db->get_table(table_query->table_name.c_str());
                if(table != nullptr) {
                    std::vector<std::vector<std::string>> result{};
                    status = table->process_query(std::move(table_query));
                }
            }
        }
END_QUERY:
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> ms = t2 - t1;
        std::string duration_str = DateTime::duration(ms.count());
        /*while(db != nullptr && db->thread_worker != nullptr && db->thread_worker->busy) {
            status = db->thread_worker->terminated_status;
        }*/
        if(status == kvdb::Status::OK_) {
            PRINT("Ok %s", duration_str.c_str());
        } else {
            if(error_str.empty()) {
                PRINT_ERROR("failed to process query", nullptr);
            } else {
                PRINT_ERROR(error_str.c_str(), nullptr);
            }
        }
        prompt();
    }

    int8_t Cli::get_command(const std::vector<std::string> &words, const std::string &str_cmd) {
        if(words.empty()) {
            return -1;
        }
        if(words.at(0) == "selectdb") {
            if(words.size() < 2) {
                return -1;
            }
            return SELECTDB;
        } else if(words.at(0) == "exit") {
            return EXIT;
        } else if(words.at(0) == "help") {
            return HELP;
        } else if(words.at(0) == "queryload") {
            return QUERYLOAD;
        }
        std::vector<std::string> words1 = split_string(words.at(0), '.');
        if(words1.empty() || words1.size() <= 1) {
            return -1;
        }
        if(words1.size() > 1) {
            words1 = split_string(words1.at(1), '(');
            return TableQuery::get_op(words1.at(0));
        }
        return -1;
    }

    void Cli::print_help() {
        PRINT("%s",
              "Help:\n"
              "    selectdb /path/database_name\t\t\topen/create a directory as a database\n"
              "    table_name.put(key=value[,...])\t\t\tsave/update row(s) in the table\n"
              "    table_name.get([key=value,...])\t\t\tsearch row(s) in the table\n"
              "    table_name.delete([key=value,...])\t\tdelete row(s) in the table\n"
              "    queryload /path/dump.txt\t\t\t\tload and process queries from a file\n"
              "    help\t\t\t\t\t\t\t\t\tprint this help message\n"
              "    exit\t\t\t\t\t\t\t\t\texit this command line interface"
        );
        prompt();
    }

    std::vector<std::string> Cli::split_string(const std::string &text, char sep) {
        std::vector<std::string> tokens;
        std::size_t start = 0, end = 0;
        while ((end = text.find(sep, start)) != std::string::npos) {
            tokens.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        tokens.push_back(text.substr(start));
        return tokens;
    }

    bool Cli::is_table_op(int8_t cmd) {
        return (cmd == TableQuery::PUT_ || cmd == TableQuery::GET_ || cmd == TableQuery::DELETE_);
    }

} // namespace kvdb

