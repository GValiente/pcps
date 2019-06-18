/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_normal_splitter.h"

#include "pcps_cloud.h"
#include "pcps_logger.h"
#include "pcps_context.h"
#include "pcps_epsilon.h"
#include "pcps_tweak_me.h"
#include "pcps_device_cloud.h"
#include "pcps_normal_region.h"
#include "pcps_thrust_normal_splitter.h"

namespace pcps
{

static_assert(sizeof(Point) == sizeof(float4), "");

bool NormalSplitter::_getMean(const DeviceCloud& normalDeviceCloud, const NormalRegion& normalRegion, Point& mean,
                              int& numValidNormals, Context& context) const
{
    const Cloud& normalCloud = normalDeviceCloud.getHostCloud();

    if(normalRegion.width * normalRegion.height < PCPS_CUDA_SPLITTER_MAX_CPU_REGION_AREA)
    {
        _getCpuMean(normalCloud, normalRegion, mean, numValidNormals);
    }
    else
    {
        pcps_thrust::getNormalMean(normalDeviceCloud.getDeviceData(), normalRegion.x, normalRegion.y,
                                   normalRegion.width, normalRegion.height, normalCloud.width,
                                   reinterpret_cast<float4&>(mean), numValidNormals, *context.allocator);
    }

    return true;
}

bool NormalSplitter::_getStdDev(const DeviceCloud& normalDeviceCloud, const NormalRegion& normalRegion,
                                const Point& mean, int numValidNormals, float& stdDev, Context& context) const
{
    const Cloud& normalCloud = normalDeviceCloud.getHostCloud();

    if(normalRegion.width * normalRegion.height < PCPS_CUDA_SPLITTER_MAX_CPU_REGION_AREA)
    {
        _getCpuStdDev(normalCloud, normalRegion, mean, numValidNormals, stdDev);
    }
    else
    {
        pcps_thrust::getNormalStdDev(normalDeviceCloud.getDeviceData(), normalRegion.x, normalRegion.y,
                                     normalRegion.width, normalRegion.height, normalCloud.width,
                                     reinterpret_cast<const float4&>(mean), numValidNormals, stdDev, *context.allocator);
    }

    return true;
}

}
