/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_normal_splitter.h"

#include "pcps_device_cloud.h"

namespace pcps
{

bool NormalSplitter::_getMean(const DeviceCloud& normalDeviceCloud, const NormalRegion& normalRegion, Point& mean,
                              int& numValidNormals, Context&) const
{
    _getCpuMean(normalDeviceCloud.getHostCloud(), normalRegion, mean, numValidNormals);
    return true;
}

bool NormalSplitter::_getStdDev(const DeviceCloud& normalDeviceCloud, const NormalRegion& normalRegion,
                                const Point& mean, int numValidNormals, float& stdDev, Context&) const
{
    _getCpuStdDev(normalDeviceCloud.getHostCloud(), normalRegion, mean, numValidNormals, stdDev);
    return true;
}

}
