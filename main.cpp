/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#include <system_error>
#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#define OS_WINDOWS
#endif
#include "cli/Cli.h"

int main(int argc, char *argv[]) {
    kvdb::Cli cli;
    cli.prompt();
    return 0;
}