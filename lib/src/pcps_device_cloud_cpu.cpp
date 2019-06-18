/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_device_cloud.h"

#include "pcps_cloud.h"
#include "pcps_logger.h"

namespace pcps
{

DeviceCloud::DeviceCloud(const Cloud& hostCloud, Context&) :
    _hostCloud(const_cast<Cloud*>(&hostCloud)),
    _deviceData(_hostCloud->points.data()),
    _constHostCloud(true)
{
}

DeviceCloud::DeviceCloud(Cloud& hostCloud, Context&) :
    _hostCloud(&hostCloud),
    _deviceData(hostCloud.points.data()),
    _constHostCloud(false)
{
}

DeviceCloud::~DeviceCloud() noexcept
{
}

bool DeviceCloud::updateHostCloud(Context&)
{
    if(_constHostCloud)
    {
        PCPS_LOG_ERROR << "Host cloud is const" << std::endl;
        return false;
    }

    return true;
}

}
