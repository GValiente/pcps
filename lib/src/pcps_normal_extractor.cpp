/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_normal_extractor.h"

#include <cmath>
#include "pcps_cloud.h"
#include "pcps_device_cloud.h"
#include "pcps_logger.h"
#include "pcps_epsilon.h"

namespace pcps
{

float NormalExtractor::getSearchRadius() const noexcept
{
    return _searchRadius;
}

bool NormalExtractor::setSearchRadius(float searchRadius) noexcept
{
    if(searchRadius <= epsilon)
    {
        PCPS_LOG_ERROR << "Invalid searchRadius: " << searchRadius << std::endl;
        return false;
    }

    _searchRadius = searchRadius;
    return true;
}

bool NormalExtractor::flipNormals() const noexcept
{
    return _flipNormals;
}

void NormalExtractor::setFlipNormals(bool flip) noexcept
{
    _flipNormals = flip;
}

const Point& NormalExtractor::getFlipViewPoint() const noexcept
{
    return _flipViewPoint;
}

void NormalExtractor::setFlipViewPoint(const Point& flipViewPoint) noexcept
{
    _flipViewPoint = flipViewPoint;
}

bool NormalExtractor::extract(const Cloud& inputPointCloud, Cloud& outputNormalCloud, Context& context) const
{
    int neighborLevels = 0;

    if(! _getNeighborLevels(inputPointCloud, neighborLevels))
    {
        PCPS_LOG_ERROR << "Neighbor levels calculation failed" << std::endl;
        return false;
    }

    outputNormalCloud.width = inputPointCloud.width;
    outputNormalCloud.height = inputPointCloud.height;
    outputNormalCloud.sensorOrigin = inputPointCloud.sensorOrigin;
    outputNormalCloud.points.resize(inputPointCloud.points.size());

    DeviceCloud inputPointDeviceCloud(inputPointCloud, context);
    DeviceCloud outputNormalDeviceCloud(outputNormalCloud, context);
    void* outputNormalDeviceData = outputNormalDeviceCloud.getDeviceData();

    if(! outputNormalDeviceData)
    {
        PCPS_LOG_ERROR << "Output normal cloud device data retrieve failed" << std::endl;
        return false;
    }

    if(! _extract(inputPointDeviceCloud, neighborLevels, outputNormalDeviceData, context))
    {
        PCPS_LOG_ERROR << "Extraction failed" << std::endl;
        return false;
    }

    if(! outputNormalDeviceCloud.updateHostCloud(context))
    {
        PCPS_LOG_ERROR << "Output normal cloud update failed" << std::endl;
        return false;
    }

    return true;
}

bool NormalExtractor::extract(const DeviceCloud& inputPointDeviceCloud, DeviceCloud& outputNormalDeviceCloud,
                              Context& context) const
{
    const Cloud& inputPointCloud = inputPointDeviceCloud.getHostCloud();
    const Cloud& outputNormalCloud = outputNormalDeviceCloud.getHostCloud();
    int neighborLevels = 0;

    if(! _getNeighborLevels(inputPointCloud, neighborLevels))
    {
        PCPS_LOG_ERROR << "Neighbor levels calculation failed" << std::endl;
        return false;
    }

    if(! outputNormalCloud.hasValidSize())
    {
        PCPS_LOG_ERROR << "Output normal cloud size is invalid" << std::endl;
        return false;
    }

    if(inputPointCloud.width != outputNormalCloud.width || inputPointCloud.height != outputNormalCloud.height)
    {
        PCPS_LOG_ERROR << "Input point cloud and output normal cloud sizes are different" << std::endl;
        return false;
    }

    void* outputNormalDeviceData = outputNormalDeviceCloud.getDeviceData();

    if(! outputNormalDeviceData)
    {
        PCPS_LOG_ERROR << "Output normal cloud device data retrieve failed" << std::endl;
        return false;
    }

    if(! _extract(inputPointDeviceCloud, neighborLevels, outputNormalDeviceData, context))
    {
        PCPS_LOG_ERROR << "Extraction failed" << std::endl;
        return false;
    }

    return true;
}

float NormalExtractor::_getCellDistance(const Cloud& cloud) noexcept
{
    int rows = cloud.height;
    int cols = cloud.width;

    for(int upRowIndex = 0; upRowIndex < rows / 2; ++upRowIndex)
    {
        for(int colIndex = 0; colIndex < cols; ++colIndex)
        {
            const Point& upPoint = cloud.at(colIndex, upRowIndex);

            if(upPoint.isFinite())
            {
                int downRowIndex = rows - upRowIndex - 1;
                const Point& downPoint = cloud.at(colIndex, downRowIndex);

                if(downPoint.isFinite())
                {
                    float distanceX = upPoint.x - downPoint.x;
                    float distanceY = upPoint.y - downPoint.y;
                    float distance = std::sqrt((distanceX * distanceX) + (distanceY * distanceY));
                    int cellCount = downRowIndex - upRowIndex;
                    return distance / cellCount;
                }
            }
        }
    }

    for(int rowIndex = 0; rowIndex < rows; ++rowIndex)
    {
        for(int leftColIndex = 0; leftColIndex < cols / 2; ++leftColIndex)
        {
            const Point& leftPoint = cloud.at(leftColIndex, rowIndex);

            if(leftPoint.isFinite())
            {
                int rightColIndex = cols - leftColIndex - 1;
                const Point& rightPoint = cloud.at(rightColIndex, rowIndex);

                if(rightPoint.isFinite())
                {
                    float distanceX = leftPoint.x - rightPoint.x;
                    float distanceY = leftPoint.y - rightPoint.y;
                    float distance = std::sqrt((distanceX * distanceX) + (distanceY * distanceY));
                    int cellCount = rightColIndex - leftColIndex;
                    return distance / cellCount;
                }
            }
        }
    }

    for(int rowIndex = 0; rowIndex < rows; ++rowIndex)
    {
        for(int colIndex = 0; colIndex < cols -1; ++colIndex)
        {
            const Point& point = cloud.at(colIndex, rowIndex);

            if(point.isFinite())
            {
                const Point& rightPoint = cloud.at(colIndex + 1, rowIndex);

                if(rightPoint.isFinite())
                {
                    float distanceX = point.x - rightPoint.x;
                    float distanceY = point.y - rightPoint.y;
                    return std::sqrt((distanceX * distanceX) + (distanceY * distanceY));
                }

                const Point& downPoint = cloud.at(colIndex, rowIndex + 1);

                if(downPoint.isFinite())
                {
                    float distanceX = point.x - downPoint.x;
                    float distanceY = point.y - downPoint.y;
                    return std::sqrt((distanceX * distanceX) + (distanceY * distanceY));
                }
            }
        }
    }

    return 0;
}

bool NormalExtractor::_getNeighborLevels(const Cloud& cloud, int& neighborLevels) const noexcept
{
    if(cloud.points.empty())
    {
        PCPS_LOG_ERROR << "Point cloud is empty" << std::endl;
        return false;
    }

    if(! cloud.hasValidSize())
    {
        PCPS_LOG_ERROR << "Point cloud size is invalid" << std::endl;
        return false;
    }

    float cellDistance = _getCellDistance(cloud);
    neighborLevels = 0;

    if(cellDistance > 0)
    {
        neighborLevels = std::max(1, int(_searchRadius / cellDistance));
    }

    if(neighborLevels <= 0)
    {
        PCPS_LOG_ERROR << "Cell distance calculation failed: " << cellDistance << std::endl;
        return false;
    }

    return true;
}

}
