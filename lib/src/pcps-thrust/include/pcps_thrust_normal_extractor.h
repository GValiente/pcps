/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_THRUST_NORMAL_EXTRACTOR_H
#define PCPS_THRUST_NORMAL_EXTRACTOR_H

#include "pcps_thrust.h"

namespace pcps_thrust
{
    class CachedAllocator;

    void computeNormals(const void* inputPoints, float4 flipViewPoint, int cols, int rows, int neighborLevels,
                        float epsilon, void* outputNormals, CachedAllocator& allocator);
}

#endif
