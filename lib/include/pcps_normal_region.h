/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_NORMAL_REGION_H
#define PCPS_NORMAL_REGION_H

#include "pcps_point.h"

namespace pcps
{

/**
 * @brief A region of normals with low standard deviation.
 */
class NormalRegion
{

public:
    Point normal; /**< Normals mean. */
    int x; /**< x-coordinate of the regions's left edge. */
    int y; /**< y-coordinate of the regions's top edge. */
    int width; /**< Width of the region. */
    int height; /**< Height of the region. */

    /**
     * @brief Indicates if this region is equal to the given one.
     * @param other The other region.
     * @param precision Upper bound on the relative error due to rounding in floating point arithmetic.
     * @return true if this region is equal to the given one; false otherwise.
     */
    bool isEqualTo(const NormalRegion& other, float precision) const noexcept;

    /**
     * @brief Indicates if this region is in contact with the given one.
     */
    bool isTouching(const NormalRegion& other) const noexcept;

    /**
     * @brief Returns true if this region is equal to the given one; false otherwise.
     */
    bool operator==(const NormalRegion& other) const noexcept;

    /**
     * @brief Returns true if this region is not equal to the given one; false otherwise.
     */
    bool operator!=(const NormalRegion& other) const noexcept;
};

}

#endif
