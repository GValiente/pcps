/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_normal_extractor.h"

#include "pcps_cloud.h"
#include "pcps_context.h"
#include "pcps_epsilon.h"
#include "pcps_device_cloud.h"
#include "pcps_thrust_normal_extractor.h"

namespace pcps
{

static_assert(sizeof(Point) == sizeof(float4), "");

bool NormalExtractor::_extract(const DeviceCloud& inputPointDeviceCloud, int neighborLevels,
                               void* outputNormalDeviceData, Context& context) const
{
    const Cloud& inputPointCloud = inputPointDeviceCloud.getHostCloud();
    const Point& sensorOrigin = inputPointCloud.sensorOrigin;
    Point flipViewPoint = _flipNormals ? _flipViewPoint : sensorOrigin;
    int cols = inputPointCloud.width;
    int rows = inputPointCloud.height;
    float4 cudaFlipViewPoint{ flipViewPoint.x, flipViewPoint.y, flipViewPoint.z, 0 };
    pcps_thrust::computeNormals(inputPointDeviceCloud.getDeviceData(), cudaFlipViewPoint, cols, rows, neighborLevels,
                                epsilon, outputNormalDeviceData, *context.allocator);
    return true;
}

}
