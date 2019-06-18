/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_point.h"

#include <cmath>
#include <limits>
#include "pcps_epsilon.h"

namespace pcps
{

Point Point::invalid() noexcept
{
    float nan = std::numeric_limits<float>::quiet_NaN();
    return { nan, nan, nan, 0 };
}

bool Point::isFinite() const noexcept
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

bool Point::isEqualTo(const Point& other, float precision) const noexcept
{
    if(isFinite())
    {
        if(other.isFinite())
        {
            return std::abs(x - other.x) < precision &&
                    std::abs(y - other.y) < precision &&
                    std::abs(z - other.z) < precision;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return ! other.isFinite();
    }
}

Point Point::cross(const Point& other) const noexcept
{
    return Point{ y * other.z - other.y * z, other.x * z - x * other.z, x * other.y - other.x * y, 0 };
}

void Point::operator*=(float value) noexcept
{
    x *= value;
    y *= value;
    z *= value;
}

void Point::operator/=(float value) noexcept
{
    x /= value;
    y /= value;
    z /= value;
}

bool Point::operator==(const Point& other) const noexcept
{
    return isEqualTo(other, epsilon);
}

bool Point::operator!=(const Point& other) const noexcept
{
    return ! (*this == other);
}

}
