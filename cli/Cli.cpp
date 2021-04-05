/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <iostream>
#include "Cli.h"
#include "../utils/log.hpp"

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::chrono::duration;

namespace kvdb {

    Cli::Cli() = default;

    void Cli::prompt() {
        std::cout << (db != nullptr && db->opened ? db->name : "") << "> ";
        getline(std::cin, command);
        std::vector<std::string> words = split_string(command, ' ');
        auto t1 = high_resolution_clock::now();
        kvdb::Status status = kvdb::ERROR;
        int8_t cmd = get_command(words, command);
        if(cmd == -1) {
            print_help();
            return;
        }
        if(cmd == EXIT) {
            exit(EXIT_SUCCESS);
        }
        if(cmd == SELECTDB) {
            if(db != nullptr) {
                db->close();
                db.reset();
            }
            db = std::make_unique<Database>(words.at(1));
            if(!db->open()) {
                ERROR("%s", "failed to open database");
                prompt();
                return;
            }
            status = kvdb::OK;
            PRINT("database '%s' selected", db->name.c_str());
        } else if(cmd == Action::PUT || cmd == Action::GET || cmd == Action::DELETE) {
            if(db == nullptr || !db->opened) {
                ERROR("%s", "No database selected. Please open a database first.\nTry: selectdb your_database_name");
                prompt();
                return;
            }
            //std::unique_ptr<Action> table_action = Action::parse(words.at(0));
            std::unique_ptr<Action> table_action = Action::parse(command);
            if(table_action != nullptr) {
                auto it = db->tables.find(table_action->table_name);
                Table *table = nullptr;
                if(it == db->tables.end()) {
                    db->tables.insert(std::pair<std::string, std::unique_ptr<Table>>(table_action->table_name, std::make_unique<Table>(table_action->table_name, db->path)));
                    it = db->tables.find(table_action->table_name);
                    if(it != db->tables.end()) {
                        table = it->second.get();
                    }
                } else {
                    table = it->second.get();
                }
                if(table != nullptr) {
                    std::vector<std::vector<std::string>> result{};
                    status = it->second->process_action(table_action->action, table_action->key_values, &result);
                }
            }
        }
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> ms = t2 - t1;
        std::string duration_str = DateTime::duration(ms.count());
        if(status == kvdb::OK) {
            PRINT("Ok %s", duration_str.c_str());
        } else {
            ERROR("%s", "failed to process the query");
        }
        prompt();
    }

    int8_t Cli::get_command(const std::vector<std::string> &words, const std::string &str_cmd) {
        /*auto it = cmds.find(words.at(0));
        if(words.empty()) {
            return -1;
        }
        if(it != cmds.end()) {
            return it->second;
        }*/
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
        }
        std::vector<std::string> words1 = split_string(words.at(0), '.');
        if(words1.empty() || words1.size() <= 1) {
            return -1;
        }
        if(words1.size() > 1) {
            words1 = split_string(words1.at(1), '(');
            return Action::get_action(words1.at(0));
        }
        return -1;
    }

    void Cli::print_help() {
        PRINT("%s",
              "Help:\n"
              "    selectdb database_name\t\t\t\t\t\t\topen/create a directory like a database\n"
              "    table_name.put(field=value[,...])\t\t\t\tsave/update value(s) in the table\n"
              "    table_name.get([field=value,...])\t\t\t\tsearch value(s) in the table\n"
              "    table_name.delete(field=value[,...])\t\t\tdelete value(s) in the table\n"
              "    exit\t\t\t\t\t\t\t\t\t\t\texit this command line interface"
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

} // namespace kvdb

