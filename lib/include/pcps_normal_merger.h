/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_NORMAL_MERGER_H
#define PCPS_NORMAL_MERGER_H

#include <vector>
#include <unordered_set>
#include "pcps_point.h"

namespace pcps
{

class Plane;
class NormalRegion;

/**
 * @brief Merges the given normal regions into planes (groups) with low standard deviation.
 */
class NormalMerger
{

public:
    /**
     * @brief Retrieves the standard deviation by which two normal regions are always merged.
     */
    float getMinimumStdDsvThreshold() const noexcept;

    /**
     * @brief Retrieves the standard deviation by which two normal regions can be merged.
     */
    float getMaximumStdDsvThreshold() const noexcept;

    /**
     * @brief Sets the standard deviation the determine if two normal regions can be merged.
     * @param minimumStdDsvThreshold Standard deviation in radians by which two normal regions are always merged (epsilon..inf).
     * @param maximumStdDsvThreshold Standard deviation in radians by which two normal regions can be merged (minimumStdDsvThreshold..inf).
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool setStdDsvThresholds(float minimumStdDsvThreshold, float maximumStdDsvThreshold) noexcept;

    /**
     * @brief Merges the given normal regions into planes (groups) with low standard deviation.
     * @param normalRegions Input normal regions.
     * @param planes Stores the output planes (groups) with low standard deviation.
     */
    void merge(const std::vector<NormalRegion>& normalRegions, std::vector<Plane>& planes);

private:
    ///@cond INTERNAL

    struct Group
    {
        std::vector<std::size_t> regionIndexes;
        std::unordered_set<std::size_t> neighborGroupIndexes;
        Point normal;
        int area;
    };

    std::vector<Group> _groups;
    std::vector<int> _pointCloudIds;
    float _minimumStdDsvThreshold = (5 * 3.14159265358979323846f) / 180;
    float _maximumStdDsvThreshold = (25 * 3.14159265358979323846f) / 180;

    void _setupGroups(const std::vector<NormalRegion>& normalRegions);

    bool _getBestNeighborGroupIndex(std::size_t groupIndex, std::size_t& bestNeighborGroupIndex) const noexcept;

    void _mergeGroups(std::size_t aIndex, std::size_t bIndex);

    void _buildPlanes(const std::vector<NormalRegion>& normalRegions, std::vector<Plane>& planes) const;

    ///@endcond
};

}

#endif
