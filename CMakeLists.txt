cmake_minimum_required(VERSION 3.17)
project(db)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

add_executable(kvdb main.cpp
        utils/log.hpp
        utils/definies.h
        tree/BTree.cpp tree/BTree.h
        tree/Node.cpp tree/Node.h)