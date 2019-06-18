/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_device_cloud.h"

#include "pcps_cloud.h"
#include "pcps_logger.h"
#include "pcps_thrust_cloud.h"

namespace pcps
{

static_assert(sizeof(Point) == sizeof(float4), "");

DeviceCloud::DeviceCloud(const Cloud& hostCloud, Context& context) :
    DeviceCloud(const_cast<Cloud&>(hostCloud), context)
{
    _constHostCloud = true;
}

DeviceCloud::DeviceCloud(Cloud& hostCloud, Context&) :
    _hostCloud(&hostCloud),
    _constHostCloud(false)
{
    const std::vector<Point>& hostPoints = hostCloud.points;
    auto hostPointsPtr = reinterpret_cast<const float4*>(hostPoints.data());
    _deviceData = pcps_thrust::createDevicePoints(hostPointsPtr, hostPoints.size());
}

DeviceCloud::~DeviceCloud() noexcept
{
    pcps_thrust::destroyDevicePoints(_deviceData);
    _deviceData = nullptr;
}

bool DeviceCloud::updateHostCloud(Context&)
{
    if(_constHostCloud)
    {
        PCPS_LOG_ERROR << "Host cloud is const" << std::endl;
        return false;
    }

    std::vector<Point>& hostPoints = _hostCloud->points;
    auto hostPointsPtr = reinterpret_cast<float4*>(hostPoints.data());
    pcps_thrust::updateHostPoints(_deviceData, hostPointsPtr);
    return true;
}

}
