/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_organizer.h"

#include <cmath>
#include "pcps_cloud.h"
#include "pcps_logger.h"
#include "pcps_context.h"
#include "pcps_epsilon.h"
#include "pcps_thrust_organizer.h"

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

    thrust::host_vector<float2> points2D(numPoints);

    for(std::size_t index = 0; index < numPoints; ++index)
    {
        const Point& inputPoint = inputPoints[index];
        float2& outputPoint = points2D[index];
        outputPoint.x = inputPoint.x;
        outputPoint.y = inputPoint.y;
    }

    thrust::host_vector<int> indices;
    int width, height;
    pcps_thrust::organize(points2D, _maximumWidth, _maximumHeight, epsilon, width, height, indices, *context.allocator);

    std::vector<Point>& outputPoints = outputPointCloud.points;
    outputPointCloud.width = width;
    outputPointCloud.height = height;
    outputPointCloud.sensorOrigin = inputPointCloud.sensorOrigin;
    outputPoints.resize(std::size_t(width * height), Point::invalid());

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
