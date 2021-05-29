/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#define VERSION "1.0.2"

#if defined(_WIN32) || defined(WIN32)
#define OS_WINDOWS 1
#endif

#ifdef OS_WINDOWS
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/\0"
#endif

#define BTREE_MAX_DEGREE 5
typedef unsigned char byte;

namespace kvdb {
    enum class Status {OK_,ERROR_};
}