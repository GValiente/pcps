/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_THRUST_ORGANIZER_H
#define PCPS_THRUST_ORGANIZER_H

#include "pcps_thrust.h"

namespace pcps_thrust
{
    class CachedAllocator;

    void organize(const thrust::host_vector<float2>& points2D, int maximumWidth, int maximumHeight, float epsilon,
                  int& width, int& height, thrust::host_vector<int>& indices, CachedAllocator& allocator);
}

#endif
