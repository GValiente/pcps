/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_POINT_CLOUD_H
#define PCPS_VW_POINT_CLOUD_H

#include "pcps_cloud.h"

namespace pcps
{
namespace vw
{

class PointCloud
{

public:
    Cloud cloud;
    float minZ = 0;
    float maxZ = 0;
};

}
}

#endif
