/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_organizer.h"

#include <cmath>
#include <limits>
#include "pcps_cloud.h"
#include "pcps_logger.h"
#include "pcps_epsilon.h"

namespace pcps
{

bool Organizer::organize(const Cloud& inputPointCloud, Cloud& outputPointCloud, Context&) const
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

    // Retrieve bounding box and minimum distance between points:

    float maxValue = std::numeric_limits<float>::max();
    float minX = maxValue;
    float minY = maxValue;
    float maxX = -maxValue;
    float maxY = -maxValue;
    float minDistance = maxValue;

    for(std::size_t index = 0; index < numPoints; ++index)
    {
        const Point& point = inputPoints[index];
        minX = std::min(minX, point.x);
        maxX = std::max(maxX, point.x);
        minY = std::min(minY, point.y);
        maxY = std::max(maxY, point.y);

        for(std::size_t otherIndex = index + 1; otherIndex < numPoints; ++otherIndex)
        {
            const Point& otherPoint = inputPoints[otherIndex];
            float distanceX = point.x - otherPoint.x;
            float distanceY = point.y - otherPoint.y;
            float distance = (distanceX * distanceX) + (distanceY * distanceY);
            minDistance = std::min(distance, minDistance);
        }
    }

    // Retrieve output cloud size and cell size:

    float distanceX = maxX - minX;
    float distanceY = maxY - minY;
    int width, height;
    minDistance = std::sqrt(minDistance);

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
    outputPointCloud.width = width;
    outputPointCloud.height = height;
    outputPointCloud.sensorOrigin = inputPointCloud.sensorOrigin;

    // Fill output cloud points vector:

    std::vector<Point>& outputPoints = outputPointCloud.points;
    outputPoints.resize(std::size_t(width * height), Point::invalid());

    for(const Point& inputPoint : inputPoints)
    {
        float relativeX = inputPoint.x - minX;
        float relativeY = inputPoint.y - minY;
        auto col = int(relativeX * cellSizeInv);
        auto row = int(relativeY * cellSizeInv);
        int index = (row * width) + col;
        Point& outputPoint = outputPoints[std::size_t(index)];

        if(! std::isfinite(outputPoint.z) || inputPoint.z > outputPoint.z)
        {
            outputPoint = inputPoint;
        }
    }

    return true;
}

}
