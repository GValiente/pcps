/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_device_cloud.h"

#include "pcps_logger.h"

namespace pcps
{

DeviceCloud::DeviceCloud(DeviceCloud&& other) noexcept :
    _hostCloud(nullptr),
    _deviceData(nullptr),
    _constHostCloud(false)
{
    *this = std::move(other);
}

DeviceCloud& DeviceCloud::operator=(DeviceCloud&& other) noexcept
{
    std::swap(_hostCloud, other._hostCloud);
    std::swap(_deviceData, other._deviceData);
    std::swap(_constHostCloud, other._constHostCloud);
    return *this;
}

void* DeviceCloud::getDeviceData() noexcept
{
    if(_constHostCloud)
    {
        PCPS_LOG_ERROR << "Host cloud is const" << std::endl;
        return nullptr;
    }

    return _deviceData;
}

}
