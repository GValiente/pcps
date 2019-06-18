/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_NORMAL_SPLITTER_H
#define PCPS_NORMAL_SPLITTER_H

#include <vector>

namespace pcps
{

class Point;
class Cloud;
class Context;
class DeviceCloud;
class NormalRegion;

/**
 * @brief Splits a given organized normal cloud into regions with low standard deviation.
 *
 * This class expects the output of NormalExtractor as input.
 */
class NormalSplitter
{

public:
    /**
     * @brief Retrieves the minimum width of each output region.
     */
    int getMinimumRegionWidth() const noexcept;

    /**
     * @brief Sets the minimum width of each output region.
     * @param minimumRegionWidth Region width in cells [1..inf).
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool setMinimumRegionWidth(int minimumRegionWidth) noexcept;

    /**
     * @brief Retrieves the minimum height of each output region.
     */
    int getMinimumRegionHeight() const noexcept;

    /**
     * @brief Sets the minimum height of each output region.
     * @param minimumRegionHeight Region height in cells [1..inf).
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool setMinimumRegionHeight(int minimumRegionHeight) noexcept;

    /**
     * @brief Retrieves the minimum standard deviation of each output region.
     */
    float getStdDsvThreshold() const noexcept;

    /**
     * @brief Sets the minimum standard deviation of each output region.
     * @param stdDsvThreshold Minimum standard deviation in radians (epsilon..inf).
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool setStdDsvThreshold(float stdDsvThreshold) noexcept;

    /**
     * @brief Splits a given organized normal cloud into regions with low standard deviation.
     * @param inputNormalCloud Input organized normal cloud.
     * @param outputNormalRegions Stores the output regions with low standard deviation.
     * @param context Compute context.
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool split(const Cloud& inputNormalCloud, std::vector<NormalRegion>& outputNormalRegions, Context& context) const;

    /**
     * @brief Splits a given organized normal cloud into regions with low standard deviation.
     * @param inputNormalDeviceCloud Input organized normal device cloud.
     * @param outputNormalRegions Stores the output regions with low standard deviation.
     * @param context Compute context.
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool split(const DeviceCloud& inputNormalDeviceCloud, std::vector<NormalRegion>& outputNormalRegions,
               Context& context) const;

private:
    ///@cond INTERNAL

    int _minimumRegionWidth = 2;
    int _minimumRegionHeight = 2;
    float _stdDsvThreshold = (3 * 3.14159265358979323846f) / 180;

    bool _getMean(const DeviceCloud& normalDeviceCloud, const NormalRegion& normalRegion, Point& mean,
                  int& numValidNormals, Context& context) const;

    bool _getStdDev(const DeviceCloud& normalDeviceCloud, const NormalRegion& normalRegion, const Point& mean,
                    int numValidNormals, float& stdDev, Context& context) const;

    bool _split(const DeviceCloud& inputNormalDeviceCloud, const NormalRegion& normalRegion,
                std::vector<NormalRegion>& outputNormalRegions, Context& context) const;

    void _getCpuMean(const Cloud& normalCloud, const NormalRegion& normalRegion, Point& mean,
                     int& numValidNormals) const;

    void _getCpuStdDev(const Cloud& normalCloud, const NormalRegion& normalRegion, const Point& mean,
                       int numValidNormals, float& stdDev) const;

    ///@endcond
};

}

#endif
