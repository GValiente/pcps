/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_ORGANIZER_H
#define PCPS_ORGANIZER_H

namespace pcps
{

class Cloud;
class Context;

/**
 * @brief Organizes a disorganized point cloud.
 *
 * An organized point cloud dataset is the name given to point clouds that resemble an organized image (or matrix)
 * like structure, where the data is split into rows and columns.
 *
 * http://pointclouds.org/documentation/tutorials/basic_structures.php
 */
class Organizer
{

public:
    /**
     * @brief Disorganizes an organized point cloud.
     * @param inputPointCloud Input organized point cloud.
     * @param outputPointCloud Stores the output disorganized point cloud.
     */
    static void disorganize(const Cloud& inputPointCloud, Cloud& outputPointCloud);

    /**
     * @brief Retrieves the maximum width of the output organized point cloud.
     */
    int getMaximumWidth() const noexcept;

    /**
     * @brief Sets the maximum width of the output organized point cloud.
     * @param maximumWidth Maximum width in cells [1..inf).
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool setMaximumWidth(int maximumWidth) noexcept;

    /**
     * @brief Retrieves the maximum height of the output organized point cloud.
     */
    int getMaximumHeight() const noexcept;

    /**
     * @brief Sets the maximum height of the output organized point cloud.
     * @param maximumHeight Maximum height in cells [1..inf).
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool setMaximumHeight(int maximumHeight) noexcept;

    /**
     * @brief Organizes a disorganized point cloud.
     * @param inputPointCloud Input disorganized point cloud.
     * @param outputPointCloud Stores the output organized point cloud.
     * @param context Compute context.
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool organize(const Cloud& inputPointCloud, Cloud& outputPointCloud, Context& context) const;

private:
    ///@cond INTERNAL

    int _maximumWidth = 2047;
    int _maximumHeight = 2047;

    ///@endcond
};

}

#endif
