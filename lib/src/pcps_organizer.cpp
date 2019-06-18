/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_organizer.h"

#include "pcps_cloud.h"
#include "pcps_logger.h"

namespace pcps
{

void Organizer::disorganize(const Cloud& inputPointCloud, Cloud& outputPointCloud)
{
    if(! inputPointCloud.isOrganized())
    {
        inputPointCloud.copyTo(outputPointCloud);
        return;
    }

    const std::vector<Point>& inputPoints = inputPointCloud.points;
    std::vector<Point>& outputPoints = outputPointCloud.points;
    outputPoints.clear();
    outputPoints.reserve(inputPoints.size());

    for(const Point& inputPoint : inputPoints)
    {
        if(inputPoint.isFinite())
        {
            outputPoints.push_back(inputPoint);
        }
    }

    outputPointCloud.width = int(outputPoints.size());
    outputPointCloud.height = 1;
    outputPointCloud.sensorOrigin = inputPointCloud.sensorOrigin;
}

int Organizer::getMaximumWidth() const noexcept
{
    return _maximumWidth;
}

bool Organizer::setMaximumWidth(int maximumWidth) noexcept
{
    if(maximumWidth < 1)
    {
        PCPS_LOG_ERROR << "Invalid maximumWidth: " << maximumWidth << std::endl;
        return false;
    }

    _maximumWidth = maximumWidth;
    return true;
}

int Organizer::getMaximumHeight() const noexcept
{
    return _maximumHeight;
}

bool Organizer::setMaximumHeight(int maximumHeight) noexcept
{
    if(maximumHeight < 1)
    {
        PCPS_LOG_ERROR << "Invalid maximumHeight: " << maximumHeight << std::endl;
        return false;
    }

    _maximumHeight = maximumHeight;
    return true;
}

}
