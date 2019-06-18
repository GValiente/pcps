/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_VW_RESULT_H
#define PCPS_VW_RESULT_H

#include <cstdint>
#include "pcps_cloud.h"
#include "pcps_plane.h"

namespace pcps
{
namespace vw
{

class Result
{

public:
    Cloud organizationResult;
    std::int64_t organizationElapsedMcs = 0;
    Cloud normalExtractionResult;
    std::int64_t normalExtractionElapsedMcs = 0;
    std::vector<NormalRegion> normalSplitResult;
    std::int64_t normalSplitElapsedMcs = 0;
    std::vector<Plane> normalMergeResult;
    std::int64_t normalMergeElapsedMcs = 0;
};

}
}

#endif
