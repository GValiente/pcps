/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_POINT_H
#define PCPS_POINT_H

namespace pcps
{

/**
 * @brief 3D point.
 */
class Point
{

public:
    float x; /**< x-coordinate. */
    float y; /**< y-coordinate. */
    float z; /**< z-coordinate. */
    float aux; /**< Auxiliar variable used for some computations. */

    /**
     * @brief Retrieves an invalid (NaN) point.
     */
    static Point invalid() noexcept;

    /**
     * @brief Indicates if all coordinates of this point are finite.
     */
    bool isFinite() const noexcept;

    /**
     * @brief Indicates if this point is equal to the given one.
     * @param other The other point.
     * @param precision Upper bound on the relative error due to rounding in floating point arithmetic.
     * @return true if this point is equal to the given one; false otherwise.
     */
    bool isEqualTo(const Point& other, float precision) const noexcept;

    /**
     * @brief Retrieves the cross product between this point and the given one.
     */
    Point cross(const Point& other) const noexcept;

    /**
     * @brief Multiplication operator.
     */
    void operator*=(float value) noexcept;

    /**
     * @brief Division operator.
     */
    void operator/=(float value) noexcept;

    /**
     * @brief Returns true if this point is equal to the given one; false otherwise.
     */
    bool operator==(const Point& other) const noexcept;

    /**
     * @brief Returns true if this point is not equal to the given one; false otherwise.
     */
    bool operator!=(const Point& other) const noexcept;
};

}

#endif
