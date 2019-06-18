/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_organizer.h"

#include <algorithm>
#include "pcps_cloud.h"
#include "pcps_logger.h"
#include "pcps_context.h"
#include "pcps_epsilon.h"
#include "pcps_boost_compute.h"

namespace bpc = boost::compute;

namespace pcps
{

bool Organizer::organize(const Cloud& inputPointCloud, Cloud& outputPointCloud, Context& context) const
{
    if(inputPointCloud.isOrganized())
    {
        inputPointCloud.copyTo(outputPointCloud);
        return true;
    }

    const std::vector<Point>& inputPoints = inputPointCloud.points;
    std::size_t numPoints = inputPoints.size();

    if(! numPoints)
    {
        PCPS_LOG_ERROR << "Input cloud has no points" << std::endl;
        return false;
    }

    if(numPoints == 1)
    {
        inputPointCloud.copyTo(outputPointCloud);
        outputPointCloud.width = 1;
        outputPointCloud.height = 1;
        return true;
    }

    // Retrieve bounding box:

    std::vector<bpc::float2_> points2D(numPoints);

    for(std::size_t index = 0; index < numPoints; ++index)
    {
        const Point& inputPoint = inputPoints[index];
        bpc::float2_& outputPoint = points2D[index];
        outputPoint[0] = inputPoint.x;
        outputPoint[1] = inputPoint.y;
    }

    bpc::mapped_view<bpc::float2_> devicePoints2D(points2D.data(), points2D.size(), *context.context);

    BOOST_COMPUTE_FUNCTION(bpc::float4_, minMaxTransform, (bpc::float2_ point),
    {
        return (float4)(point.x, point.y, point.x, point.y);
    });

    BOOST_COMPUTE_FUNCTION(bpc::float4_, minMaxReduce, (bpc::float4_ a, bpc::float4_ b),
    {
        float minX = a.x < b.x ? a.x : b.x;
        float minY = a.y < b.y ? a.y : b.y;
        float maxX = a.z > b.z ? a.z : b.z;
        float maxY = a.w > b.w ? a.w : b.w;
        return (float4)(minX, minY, maxX, maxY);
    });

    auto maxValue = std::numeric_limits<float>::max();
    bpc::float4_ boundingBox = { maxValue, maxValue, -maxValue, -maxValue };

    try
    {
        bpc::transform_reduce(devicePoints2D.begin(), devicePoints2D.end(), &boundingBox, minMaxTransform,
                              minMaxReduce, *context.queue);
    }
    catch(const bpc::program_build_failure& programBuildFailure)
    {
        PCPS_LOG_ERROR << "Program build failure: " << std::endl;
        PCPS_LOG_ERROR << programBuildFailure.build_log() << std::endl;
        return false;
    }

    // Retrieve minimum distance between points:

    float minX = boundingBox[0];
    float minY = boundingBox[1];
    float distanceX = boundingBox[2] - minX;
    float distanceY = boundingBox[3] - minY;
    float maxDistance = maxValue;

    std::array<int, 1> distanceIntArgs = { int(numPoints) };
    bpc::mapped_view<int> deviceDistanceIntArgs(distanceIntArgs.data(), distanceIntArgs.size(), *context.context);
    std::array<float, 1> distanceFloatArgs = { maxDistance };
    bpc::mapped_view<float> deviceDistanceFloatArgs(distanceFloatArgs.data(), distanceFloatArgs.size(), *context.context);

    BOOST_COMPUTE_CLOSURE(float, distanceTransform, (int index),
                          (devicePoints2D, deviceDistanceIntArgs, deviceDistanceFloatArgs),
    {
        int numPoints = deviceDistanceIntArgs[0];
        float maxDistance = deviceDistanceFloatArgs[0];

        float2 point = devicePoints2D[index];
        float minDistance = maxDistance;
        ++index;

        while(index < numPoints)
        {
            float2 otherPoint = devicePoints2D[index];
            float distanceX = point.x - otherPoint.x;
            float distanceY = point.y - otherPoint.y;
            float distance = (distanceX * distanceX) + (distanceY * distanceY);

            if(distance < minDistance)
            {
                minDistance = distance;
            }

            ++index;
        }

        return minDistance;
    });

    BOOST_COMPUTE_FUNCTION(float, distanceReduce, (float a, float b),
    {
        return a < b ? a : b;
    });

    bpc::counting_iterator<int> first(0);
    bpc::counting_iterator<int> last = first + long(numPoints);
    float minDistance = maxValue;

    try
    {
        bpc::transform_reduce(first, last, &minDistance, distanceTransform, distanceReduce, *context.queue);
    }
    catch(const bpc::program_build_failure& programBuildFailure)
    {
        PCPS_LOG_ERROR << "Program build failure: " << std::endl;
        PCPS_LOG_ERROR << programBuildFailure.build_log() << std::endl;
        return false;
    }

    minDistance = std::sqrt(minDistance);

    // Retrieve output cloud size and cell size:

    int width, height;

    if(minDistance > epsilon)
    {
        width = std::min(int(distanceX / minDistance) + 1, _maximumWidth);
        height = std::min(int(distanceY / minDistance) + 1, _maximumHeight);
    }
    else
    {
        width = _maximumWidth;
        height = _maximumHeight;
    }

    float cellSize = std::max(distanceX / width, distanceY / height);
    float cellSizeInv = 1 / cellSize;
    width = int(distanceX * cellSizeInv) + 1;
    height = int(distanceY * cellSizeInv) + 1;

    // Fill output cloud points vector:

    std::vector<int> indices(numPoints, -1);
    bpc::mapped_view<int> deviceIndices(indices.data(), indices.size(), *context.context);

    std::array<int, 1> indexIntArgs = { width };
    bpc::mapped_view<int> deviceIndexIntArgs(indexIntArgs.data(), indexIntArgs.size(), *context.context);
    std::array<float, 3> indexFloatArgs = { minX, minY, cellSizeInv };
    bpc::mapped_view<float> deviceIndexFloatArgs(indexFloatArgs.data(), indexFloatArgs.size(), *context.context);

    BOOST_COMPUTE_CLOSURE(int, indexTransform, (bpc::float2_ point), (deviceIndexIntArgs, deviceIndexFloatArgs),
    {
        int width = deviceIndexIntArgs[0];
        float minX = deviceIndexFloatArgs[0];
        float minY = deviceIndexFloatArgs[1];
        float cellSizeInv = deviceIndexFloatArgs[2];

        float relativeX = point.x - minX;
        float relativeY = point.y - minY;
        int col = relativeX * cellSizeInv;
        int row = relativeY * cellSizeInv;
        return (row * width) + col;
    });

    try
    {
        bpc::transform(devicePoints2D.begin(), devicePoints2D.end(), deviceIndices.begin(), indexTransform,
                       *context.queue);
    }
    catch(const bpc::program_build_failure& programBuildFailure)
    {
        PCPS_LOG_ERROR << "Program build failure: " << std::endl;
        PCPS_LOG_ERROR << programBuildFailure.build_log() << std::endl;
        return false;
    }

    std::vector<Point>& outputPoints = outputPointCloud.points;
    outputPointCloud.width = width;
    outputPointCloud.height = height;
    outputPointCloud.sensorOrigin = inputPointCloud.sensorOrigin;
    outputPoints.resize(std::size_t(width * height), Point::invalid());
    deviceIndices.map(*context.queue);

    for(std::size_t index = 0; index < numPoints; ++index)
    {
        const Point& inputPoint = inputPoints[index];
        int outputPointIndex = indices[index];
        Point& outputPoint = outputPoints[std::size_t(outputPointIndex)];

        if(! std::isfinite(outputPoint.z) || inputPoint.z > outputPoint.z)
        {
            outputPoint = inputPoint;
        }
    }

    return true;
}

}
