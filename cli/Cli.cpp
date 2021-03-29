/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <iostream>
#include "Cli.h"
#include "../utils/log.hpp"

namespace kvdb {

    Cli::Cli() {
        cmds = {
                {"opendb", OPENDB},
                {"exit", EXIT},
        };
    }

    void Cli::prompt() {
        std::cout << (db != nullptr && db->opened ? db->name : "") << "> ";
        getline(std::cin, command);
        std::vector<std::string> words = split_string(command, ' ');
        int8_t cmd = get_command(words, command);
        if(cmd == -1) {
            print_help();
            return;
        }
        if(cmd == EXIT) {
            exit(EXIT_SUCCESS);
        }
        if(cmd == OPENDB) {
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
            PRINT("database '%s' opened", db->name.c_str());
        } else if(cmd == Action::PUT || cmd == Action::GET || cmd == Action::DELETE) {
            if(db == nullptr || !db->opened) {
                ERROR("%s", "No database selected. Please open a database first.\nTry: opendb your_database_name");
                prompt();
                return;
            }
            std::unique_ptr<Action> table_action = Action::parse(words.at(0));
            if(table_action != nullptr) {
                auto it = db->tables.find(table_action->table_name);
                if(it == db->tables.end()) {
                    db->tables.insert(std::pair<std::string, std::unique_ptr<Table>>(table_action->table_name, std::make_unique<Table>()));
                    it = db->tables.find(table_action->table_name);
                    if(it != db->tables.end()) {
                        std::string result = it->second->process_action(table_action->action, table_action->fields);
                    }
                } else {
                    std::string result = it->second->process_action(table_action->action, table_action->fields);
                }
            }
        }
        prompt();
    }

    int8_t Cli::get_command(const std::vector<std::string> &words, const std::string &str_cmd) {
        auto it = cmds.find(words.at(0));
        if(words.empty()) {
            return -1;
        }
        if(it != cmds.end()) {
            return it->second;
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
              " opendb\t\t\t\t\t\t\t\t\t\t\topen/create a directory like a database\n"
              " table_name.put([field:value,...])\t\t\tsave/update value(s) in the table\n"
              " table_name.get([field:value,...])\t\t\tsearch value(s) in the table\n"
              " table_name.delete([field:value,...])\t\t\tdelete value(s) in the table\n"
              " exit\t\t\t\t\t\t\t\t\t\t\texit this command line interface"
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

