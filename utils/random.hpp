/*
 *  Copyright Koukougnon Martial Babo, 2021.
 */
#pragma once
#include <chrono>
#include <random>
#include <climits>

static const int32_t random_int32() {
    std::random_device rd;
    std::mt19937::result_type seed = rd() ^ ((std::mt19937::result_type)
                    std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + (std::mt19937::result_type)
                    std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() );

    std::mt19937 gen(seed);
    std::uniform_int_distribution<int32_t> distrib(INT_MIN, INT_MAX);

    return distrib(gen);
}
