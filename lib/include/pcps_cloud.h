/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_CLOUD_H
#define PCPS_CLOUD_H

#include <vector>
#include "pcps_point.h"

namespace pcps
{

/**
 * @brief Stores collections of points.
 */
class Cloud
{

public:
    std::vector<Point> points; /**< Stores the collection of points. */
    Point sensorOrigin = { 0, 0, 0, 0 }; /**< Specifies the sensor acquisition origin. */
    int width = 0; /**< If the cloud is organized, stores its width in cells. */
    int height = 0; /**< If the cloud is organized, stores its height in cells. */

    /**
     * @brief Retrieves the point stored at the given cell. The cloud must be organized for this method to work.
     * @param column Cell column [0..width).
     * @param row Cell row [0..height).
     * @return The point stored at the given cell.
     */
    const Point& at(int column, int row) const noexcept;

    /**
     * @brief Retrieves the point stored at the given cell. The cloud must be organized for this method to work.
     * @param column Cell column [0..width).
     * @param row Cell row [0..height).
     * @return The point stored at the given cell.
     */
    Point& at(int column, int row) noexcept;

    /**
     * @brief Indicates if the cloud is organized.
     *
     * An organized point cloud dataset is the name given to point clouds that resemble an organized image (or matrix)
     * like structure, where the data is split into rows and columns.
     *
     * http://pointclouds.org/documentation/tutorials/basic_structures.php
     */
    bool isOrganized() const noexcept;

    /**
     * @brief Indicates if the number of stored points is equal to the cloud area.
     */
    bool hasValidSize() const noexcept;

    /**
     * @brief Copies the content of this cloud to another one.
     * @param other Output point cloud.
     */
    void copyTo(Cloud& other) const;

    /**
     * @brief Resets the state of this cloud to the default one.
     */
    void reset() noexcept;

    /**
     * @brief Returns true if this cloud is equal to the given one; false otherwise.
     */
    bool operator==(const Cloud& other) const noexcept;

    /**
     * @brief Returns true if this cloud is not equal to the given one; false otherwise.
     */
    bool operator!=(const Cloud& other) const noexcept;
};

}

#endif
