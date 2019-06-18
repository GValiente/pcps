/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_device_cloud.h"

#include "pcps_cloud.h"
#include "pcps_logger.h"
#include "pcps_context.h"
#include "pcps_boost_compute.h"

namespace bpc = boost::compute;
using DeviceView = bpc::mapped_view<bpc::float4_>;

namespace pcps
{

static_assert(sizeof(Point) == sizeof(bpc::float4_), "");

DeviceCloud::DeviceCloud(const Cloud& hostCloud, Context& context) :
    _hostCloud(const_cast<Cloud*>(&hostCloud)),
    _constHostCloud(true)
{
    const std::vector<Point>& points = hostCloud.points;
    auto pointsPtr = reinterpret_cast<const bpc::float4_*>(points.data());
    _deviceData = new DeviceView(pointsPtr, points.size(), *context.context);
}

DeviceCloud::DeviceCloud(Cloud& hostCloud, Context& context) :
    _hostCloud(&hostCloud),
    _constHostCloud(false)
{
    std::vector<Point>& points = hostCloud.points;
    auto pointsPtr = reinterpret_cast<bpc::float4_*>(points.data());
    _deviceData = new DeviceView(pointsPtr, points.size(), *context.context);
}

DeviceCloud::~DeviceCloud() noexcept
{
    auto deviceView = static_cast<DeviceView*>(_deviceData);
    delete deviceView;
    _deviceData = nullptr;
}

bool DeviceCloud::updateHostCloud(Context& context)
{
    if(_constHostCloud)
    {
        PCPS_LOG_ERROR << "Host cloud is const" << std::endl;
        return false;
    }

    auto deviceView = static_cast<DeviceView*>(_deviceData);
    deviceView->map(*context.queue);
    return true;
}

}
