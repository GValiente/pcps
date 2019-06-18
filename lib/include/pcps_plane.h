/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_PLANE_H
#define PCPS_PLANE_H

#include <vector>
#include "pcps_normal_region.h"

namespace pcps
{

/**
 * @brief Group of normal regions with low standard deviation.
 */
class Plane
{

public:
    std::vector<NormalRegion> regions; /**< Normal regions with low standard deviation. */
};

}

#endif
