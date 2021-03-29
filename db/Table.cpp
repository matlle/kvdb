/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include "Table.h"
#include "../cli/Cli.h"

namespace kvdb {

    int8_t Action::get_action(const std::string &str) {
        if (str == "put") {
            return PUT;
        }
        if (str == "get") {
            return GET;
        }
        if (str == "delete") {
            return DELETE;
        }
        return -1;
    }

    std::unique_ptr<Action> Action::parse(const std::string &str) {
        std::unique_ptr<Action> action = std::make_unique<Action>();
        std::vector<std::string> words = Cli::split_string(str, '.');
        if(words.empty() || words.size() <= 1) {
            return nullptr;
        }
        action->table_name = words.at(0);
        words = Cli::split_string(words.at(1), '(');
        if(words.empty() || words.size() <= 1) {
            return nullptr;
        }
        action->action = Action::get_action(words.at(0));
        std::string action_fields = words.at(1).substr(0, words.at(1).size() - 1);
        std::vector<std::string> fields = Cli::split_string(action_fields, ',');
        for(auto & i : fields) {
            std::vector<std::string> field = Cli::split_string(i, ':');
            action->fields.push_back(field);
        }
        return action;
    }

    Table::Table() = default;

    std::string Table::process_action(int8_t action, const std::vector<std::vector<std::string>> &fields) {
        return std::__cxx11::string();
    }
}