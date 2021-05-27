/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <system_error>
#include "cli/Cli.h"

int main(int argc, char *argv[]) {
    kvdb::Cli cli;
    cli.prompt();
    return 0;
}