/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_RANDOM_H
#define PCPS_VW_RANDOM_H

#include <cstdint>

namespace pcps
{
namespace vw
{

/**
 * @brief Fast, deterministic random number generator.
 *
 * https://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c
 */
class Random
{

public:
    using Type = std::uint32_t;

    Type get() noexcept
    {
        _x ^= _x << 16;
        _x ^= _x >> 5;
        _x ^= _x << 1;

        Type t = _x;
        _x = _y;
        _y = _z;
        _z = t ^ _x ^ _y;

        return _z;
    }

private:
    Type _x = 123456789;
    Type _y = 362436069;
    Type _z = 521288629;
};

}
}

#endif
