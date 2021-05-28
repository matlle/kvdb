/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once

#define BTREE_MAX_DEGREE 5
typedef unsigned char byte;

#ifdef OS_WINDOWS
#define PATH_SEPARATOR "\\0"
#else
#define PATH_SEPARATOR "/\0"
#endif

namespace kvdb {
    enum Status {OK, ERROR};
}