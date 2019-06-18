/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_THRUST_NORMAL_SPLITTER_H
#define PCPS_THRUST_NORMAL_SPLITTER_H

#include "pcps_thrust.h"

namespace pcps_thrust
{
    class CachedAllocator;

    void getNormalMean(const void* normals, int normalRegionX, int normalRegionY, int normalRegionWidth,
                       int normalRegionHeight, int normalsWidth, float4& mean, int& numValidNormals,
                       CachedAllocator& allocator);

    void getNormalStdDev(const void* normals, int normalRegionX, int normalRegionY, int normalRegionWidth,
                         int normalRegionHeight, int normalsWidth, float4 mean, int numValidNormals, float& stdDev,
                         CachedAllocator& allocator);
}

#endif
