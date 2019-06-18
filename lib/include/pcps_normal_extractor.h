/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_NORMAL_EXTRACTOR_H
#define PCPS_NORMAL_EXTRACTOR_H

#include "pcps_point.h"

namespace pcps
{

class Cloud;
class Context;
class DeviceCloud;

/**
 * @brief Estimates local surface normals at each point of the given organized point cloud.
 *
 * See pcl::NormalEstimation: http://pointclouds.org/documentation/tutorials/normal_estimation.php
 */
class NormalExtractor
{

public:
    /**
     * @brief Retrieves the radius of the search circumference to determine the surface normal for each point.
     */
    float getSearchRadius() const noexcept;

    /**
     * @brief Sets the radius of the search circumference to determine the surface normal for each point.
     * @param searchRadius Search radius in metres (epsilon..inf).
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool setSearchRadius(float searchRadius) noexcept;

    /**
     * @brief Indicates if the output normals must be flipped towards the view point.
     */
    bool flipNormals() const noexcept;

    /**
     * @brief Sets if the output normals must be flipped towards the view point.
     */
    void setFlipNormals(bool flip) noexcept;

    /**
     * @brief Retrieves the view point by wich the output normals must be flipped.
     */
    const Point& getFlipViewPoint() const noexcept;

    /**
     * @brief Sets the view point by wich the output normals must be flipped.
     * @param flipViewPoint Flip view point.
     */
    void setFlipViewPoint(const Point& flipViewPoint) noexcept;

    /**
     * @brief Estimates local surface normals at each point of the given organized point cloud.
     * @param inputPointCloud Input organized point cloud.
     * @param outputNormalCloud Stores the output normals for each point.
     * @param context Compute context.
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool extract(const Cloud& inputPointCloud, Cloud& outputNormalCloud, Context& context) const;

    /**
     * @brief Estimates local surface normals at each point of the given point cloud.
     * @param inputPointDeviceCloud Input organized point device cloud.
     * @param outputNormalDeviceCloud Stores the output normals for each point.
     * @param context Compute context.
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool extract(const DeviceCloud& inputPointDeviceCloud, DeviceCloud& outputNormalDeviceCloud,
                 Context& context) const;

private:
    ///@cond INTERNAL

    Point _flipViewPoint = { 0, 0, 10000, 0 };
    float _searchRadius = 0.5f;
    bool _flipNormals = true;

    static float _getCellDistance(const Cloud& cloud) noexcept;

    bool _getNeighborLevels(const Cloud& cloud, int& neighborLevels) const noexcept;

    bool _extract(const DeviceCloud& inputPointDeviceCloud, int neighborLevels, void* outputNormalDeviceData,
                  Context& context) const;

    ///@endcond

};

}

#endif
