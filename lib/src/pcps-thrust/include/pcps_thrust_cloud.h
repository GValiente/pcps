/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_THRUST_CLOUD_H
#define PCPS_THRUST_CLOUD_H

#include "pcps_thrust.h"

namespace pcps_thrust
{
    void* createDevicePoints(const float4* hostPoints, std::size_t numHostPoints);

    void updateHostPoints(const void* devicePoints, float4* hostPoints);

    void destroyDevicePoints(void* devicePoints) noexcept;
}

#endif
