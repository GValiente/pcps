/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_PLANE_SEGMENTATOR_H
#define PCPS_PLANE_SEGMENTATOR_H

#include <cstdint>
#include "pcps_cloud.h"
#include "pcps_plane.h"
#include "pcps_organizer.h"
#include "pcps_normal_extractor.h"
#include "pcps_normal_splitter.h"
#include "pcps_normal_merger.h"

namespace pcps
{

/**
 * @brief Segmentate a given point cloud into planes.
 */
class PlaneSegmentator
{

public:
    Organizer organizer; /**< Point cloud organizer. */
    NormalExtractor normalExtractor; /**< Surface normals extractor. */
    NormalSplitter normalSplitter; /**< Surface normals splitter. */
    NormalMerger normalMerger; /**< Surface normals regions merger. */

    /**
     * @brief Indicates if the intermediate results of the last segmentation must be stored.
     */
    bool storeIntermediateResults() const noexcept;

    /**
     * @brief Sets if the intermediate results of the last segmentation must be stored.
     */
    void setStoreIntermediateResults(bool store) noexcept;

    /**
     * @brief Segmentate a given point cloud into planes.
     * @param inputPointCloud Input point cloud.
     * @param outputPlanes Stores the result of the segmentation.
     * @param context Compute context.
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool segmentate(const Cloud& inputPointCloud, std::vector<Plane>& outputPlanes, Context& context);

    /**
     * @brief Segmentate a given point cloud into planes.
     * @param inputPointDeviceCloud Input point device cloud.
     * @param outputPlanes Stores the result of the segmentation.
     * @param context Compute context.
     * @return true if the operation was completed successfully; false otherwise.
     */
    bool segmentate(const DeviceCloud& inputPointDeviceCloud, std::vector<Plane>& outputPlanes, Context& context);

    /**
     * @brief Retrieves the result of the last point cloud organization.
     *
     * This result is provided only if storeIntermediateResults() is true.
     */
    const Cloud& getLastOrganizationResult() const noexcept;

    /**
     * @brief Retrieves the elapsed microseconds on the last point cloud organization.
     */
    std::int64_t getLastOrganizationElapsedMcs() const noexcept;

    /**
     * @brief Retrieves the result of the last surface normals extraction.
     *
     * This result is provided only if storeIntermediateResults() is true.
     */
    const Cloud& getLastNormalExtractionResult() const noexcept;

    /**
     * @brief Retrieves the elapsed microseconds on the last surface normals extraction.
     */
    std::int64_t getLastNormalExtractionElapsedMcs() const noexcept;

    /**
     * @brief Retrieves the result of the last surface normals split.
     *
     * This result is provided only if storeIntermediateResults() is true.
     */
    const std::vector<NormalRegion>& getLastNormalSplitResult() const noexcept;

    /**
     * @brief Retrieves the elapsed microseconds on the last surface normals split.
     */
    std::int64_t getLastNormalSplitElapsedMcs() const noexcept;

    /**
     * @brief Retrieves the elapsed microseconds on the last surface normal regions merge.
     */
    std::int64_t getLastNormalMergeElapsedMcs() const noexcept;

private:
    ///@cond INTERNAL

    Cloud _organizationResult;
    Cloud _normalExtractionResult;
    std::vector<NormalRegion> _normalSplitResult;
    std::int64_t _organizationElapsedMcs = 0;
    std::int64_t _normalExtractionElapsedMcs = 0;
    std::int64_t _normalSplitElapsedMcs = 0;
    std::int64_t _normalMergeElapsedMcs = 0;
    bool _storeIntermediateResults = false;

    bool _start(const Cloud& inputPointCloud);

    bool _segmentateImpl(const DeviceCloud& inputPointDeviceCloud, std::vector<Plane>& outputPlanes, Context& context);

    ///@endcond
};

}

#endif
