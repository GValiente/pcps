/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_plane_segmentator.h"

#include <chrono>
#include "pcps_logger.h"
#include "pcps_device_cloud.h"

namespace pcps
{

bool PlaneSegmentator::storeIntermediateResults() const noexcept
{
    return _storeIntermediateResults;
}

void PlaneSegmentator::setStoreIntermediateResults(bool store) noexcept
{
    _storeIntermediateResults = store;
}

bool PlaneSegmentator::segmentate(const Cloud& inputPointCloud, std::vector<Plane>& outputPlanes, Context& context)
{
    if(! _start(inputPointCloud))
    {
        PCPS_LOG_ERROR << "Segmentation start failed" << std::endl;
        return false;
    }

    const Cloud* pointCloudPtr;

    if(inputPointCloud.isOrganized())
    {
        pointCloudPtr = &inputPointCloud;

        if(_storeIntermediateResults)
        {
            inputPointCloud.copyTo(_organizationResult);
        }
    }
    else
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        if(! organizer.organize(inputPointCloud, _organizationResult, context))
        {
            PCPS_LOG_ERROR << "Point cloud organization failed" << std::endl;
            return false;
        }

        auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
        _organizationElapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
        pointCloudPtr = &_organizationResult;
    }

    DeviceCloud inputPointDeviceCloud(*pointCloudPtr, context);

    if(! _segmentateImpl(inputPointDeviceCloud, outputPlanes, context))
    {
        PCPS_LOG_ERROR << "Organized point cloud segmentation failed" << std::endl;
        return false;
    }

    return true;
}

bool PlaneSegmentator::segmentate(const DeviceCloud& inputPointDeviceCloud, std::vector<Plane>& outputPlanes,
                                  Context& context)
{
    const Cloud& inputPointCloud = inputPointDeviceCloud.getHostCloud();

    if(! _start(inputPointCloud))
    {
        PCPS_LOG_ERROR << "Segmentation start failed" << std::endl;
        return false;
    }

    if(inputPointCloud.isOrganized())
    {
        if(_storeIntermediateResults)
        {
            inputPointCloud.copyTo(_organizationResult);
        }

        if(! _segmentateImpl(inputPointDeviceCloud, outputPlanes, context))
        {
            PCPS_LOG_ERROR << "Organized point cloud segmentation failed" << std::endl;
            return false;
        }
    }
    else
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        if(! organizer.organize(inputPointCloud, _organizationResult, context))
        {
            PCPS_LOG_ERROR << "Point cloud organization failed" << std::endl;
            return false;
        }

        auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
        _organizationElapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();

        DeviceCloud organizedPointDeviceCloud(_organizationResult, context);

        if(! _segmentateImpl(organizedPointDeviceCloud, outputPlanes, context))
        {
            PCPS_LOG_ERROR << "Organized point cloud segmentation failed" << std::endl;
            return false;
        }
    }

    return true;
}

const Cloud& PlaneSegmentator::getLastOrganizationResult() const noexcept
{
    return _organizationResult;
}

std::int64_t PlaneSegmentator::getLastOrganizationElapsedMcs() const noexcept
{
    return _organizationElapsedMcs;
}

const Cloud& PlaneSegmentator::getLastNormalExtractionResult() const noexcept
{
    return _normalExtractionResult;
}

std::int64_t PlaneSegmentator::getLastNormalExtractionElapsedMcs() const noexcept
{
    return _normalExtractionElapsedMcs;
}

const std::vector<NormalRegion>& PlaneSegmentator::getLastNormalSplitResult() const noexcept
{
    return _normalSplitResult;
}

std::int64_t PlaneSegmentator::getLastNormalSplitElapsedMcs() const noexcept
{
    return _normalSplitElapsedMcs;
}

std::int64_t PlaneSegmentator::getLastNormalMergeElapsedMcs() const noexcept
{
    return _normalMergeElapsedMcs;
}

bool PlaneSegmentator::_start(const Cloud& inputPointCloud)
{
    _organizationResult.reset();
    _organizationElapsedMcs = 0;

    _normalExtractionResult.reset();
    _normalExtractionElapsedMcs = 0;

    _normalSplitResult.clear();
    _normalSplitElapsedMcs = 0;

    _normalMergeElapsedMcs = 0;

    if(inputPointCloud.points.empty())
    {
        PCPS_LOG_ERROR << "Input point cloud is empty" << std::endl;
        return false;
    }

    if(! inputPointCloud.hasValidSize())
    {
        PCPS_LOG_ERROR << "Input point cloud size is invalid" << std::endl;
        return false;
    }

    return true;
}

bool PlaneSegmentator::_segmentateImpl(const DeviceCloud& inputPointDeviceCloud, std::vector<Plane>& outputPlanes,
                                       Context& context)
{
    const Cloud& inputPointCloud = inputPointDeviceCloud.getHostCloud();
    _normalExtractionResult.width = inputPointCloud.width;
    _normalExtractionResult.height = inputPointCloud.height;
    _normalExtractionResult.sensorOrigin = inputPointCloud.sensorOrigin;
    _normalExtractionResult.points.resize(inputPointCloud.points.size());

    DeviceCloud outputNormalDeviceCloud(_normalExtractionResult, context);

    auto startTime = std::chrono::high_resolution_clock::now();

    if(! normalExtractor.extract(inputPointDeviceCloud, outputNormalDeviceCloud, context))
    {
        PCPS_LOG_ERROR << "Point cloud normal extraction failed" << std::endl;
        return false;
    }

    auto elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    _normalExtractionElapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();

    if(! outputNormalDeviceCloud.updateHostCloud(context))
    {
        PCPS_LOG_ERROR << "Host normal cloud update failed" << std::endl;
        return false;
    }

    startTime = std::chrono::high_resolution_clock::now();

    if(! normalSplitter.split(outputNormalDeviceCloud, _normalSplitResult, context))
    {
        PCPS_LOG_ERROR << "Normal cloud split failed" << std::endl;
        return false;
    }

    elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    _normalSplitElapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();

    startTime = std::chrono::high_resolution_clock::now();
    normalMerger.merge(_normalSplitResult, outputPlanes);
    elapsedTime = std::chrono::high_resolution_clock::now() - startTime;
    _normalMergeElapsedMcs = std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
    return true;
}

}
