cmake_minimum_required(VERSION 3.17)
project(db)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

add_executable(kvdb main.cpp
        time/DateTime.h time/DateTime.cpp
        utils/log.hpp
        utils/definies.h
        tree/BTree.cpp tree/BTree.h
        tree/Node.cpp tree/Node.h
        tree/Key.cpp tree/Key.h
        tree/Value.cpp tree/Value.h
        cli/Cli.cpp cli/Cli.h
        db/Database.cpp db/Database.h
        db/Table.cpp db/Table.h
        db/Stream.cpp db/Stream.h)