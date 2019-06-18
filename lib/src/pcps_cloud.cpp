/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_cloud.h"

#include <cmath>
#include "pcps_assert.h"

namespace pcps
{

const Point& Cloud::at(int column, int row) const noexcept
{
    return const_cast<Cloud*>(this)->at(column, row);
}

Point& Cloud::at(int column, int row) noexcept
{
    PCPS_ASSERT(column >= 0);
    PCPS_ASSERT(column < width);
    PCPS_ASSERT(row >= 0);
    PCPS_ASSERT(row < height);

    return points[std::size_t((row * width) + column)];
}

bool Cloud::isOrganized() const noexcept
{
    return (width > 1 && height > 1) || (width == 1 && height == 1);
}

bool Cloud::hasValidSize() const noexcept
{
    return int(points.size()) == width * height;
}

void Cloud::copyTo(Cloud& other) const
{
    other.points.clear();
    other.points.reserve(points.size());
    other.points.insert(other.points.end(), points.begin(), points.end());
    other.width = width;
    other.height = height;
    other.sensorOrigin = sensorOrigin;
}

void Cloud::reset() noexcept
{
    points.clear();
    sensorOrigin = Point{ 0, 0, 0, 0 };
    width = 0;
    height = 0;
}

bool Cloud::operator==(const Cloud& other) const noexcept
{
    std::size_t numPoints = points.size();

    if(numPoints != other.points.size() || width != other.width || height != other.height ||
            sensorOrigin != other.sensorOrigin)
    {
        return false;
    }

    for(std::size_t index = 0; index < numPoints; ++index)
    {
        const Point& point = points[index];
        const Point& otherPoint = other.points[index];

        if(point != otherPoint)
        {
            return false;
        }
    }

    return true;
}

bool Cloud::operator!=(const Cloud& other) const noexcept
{
    return ! (*this == other);
}

}
