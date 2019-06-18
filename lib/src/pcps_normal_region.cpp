/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_normal_region.h"

#include <cmath>
#include "pcps_epsilon.h"

namespace pcps
{

namespace
{
    bool valueInRange(int value, int min, int max) noexcept
    {
        return value >= min && value <= max;
    }
}

bool NormalRegion::isEqualTo(const NormalRegion& other, float precision) const noexcept
{
    return x == other.x && y == other.y && width == other.width && height == other.height &&
            normal.isEqualTo(other.normal, precision);
}

bool NormalRegion::isTouching(const NormalRegion& other) const noexcept
{
    if(x == other.x + other.width || x + width == other.x)
    {
        return valueInRange(y, other.y, other.y + other.height) || valueInRange(other.y, y, y + height);
    }

    if(y == other.y + other.height || y + height == other.y)
    {
        return valueInRange(x, other.x, other.x + other.width) || valueInRange(other.x, x, x + width);
    }

    return false;
}

bool NormalRegion::operator==(const NormalRegion& other) const noexcept
{
    return isEqualTo(other, epsilon);
}

bool NormalRegion::operator!=(const NormalRegion& other) const noexcept
{
    return ! (*this == other);
}

}
