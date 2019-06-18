/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_thrust_cloud.h"

#include "thrust/device_vector.h"

namespace pcps_thrust
{

void* createDevicePoints(const float4* hostPoints, std::size_t numHostPoints)
{
    auto devicePoints = new thrust::device_vector<float4>(numHostPoints);
    thrust::copy(hostPoints, hostPoints + numHostPoints, devicePoints->begin());
    return devicePoints;
}

void updateHostPoints(const void* devicePoints, float4* hostPoints)
{
    auto deviceVector = static_cast<const thrust::device_vector<float4>*>(devicePoints);
    thrust::copy(deviceVector->begin(), deviceVector->end(), hostPoints);
}

void destroyDevicePoints(void* devicePoints) noexcept
{
    auto deviceVector = static_cast<thrust::device_vector<float4>*>(devicePoints);
    delete deviceVector;
}

}
