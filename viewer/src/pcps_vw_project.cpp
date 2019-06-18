/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_project.h"

#include <pcl/io/pcd_io.h>
#include "pcps_pcl.h"
#include "pcps_logger.h"

namespace pcps
{
namespace vw
{

std::unique_ptr<Project::LoadResult> Project::loadPointCloud(const QString& pointCloudPath)
{
    pcl::PointCloud<pcl::PointXYZ> pclPointCloud;

    if(pcl::io::loadPCDFile(pointCloudPath.toStdString(), pclPointCloud))
    {
        PCPS_LOG_ERROR << "PCD point cloud load failed" << std::endl;
        return nullptr;
    }

    std::unique_ptr<PointCloud> pointCloud(new PointCloud());
    fromPCL(pclPointCloud, pointCloud->cloud);

    std::unique_ptr<Result> result = _processPointCloud(*pointCloud);

    if(! result)
    {
        PCPS_LOG_ERROR << "Loaded point cloud process failed" << std::endl;
        return nullptr;
    }

    pointCloud->minZ = std::numeric_limits<float>::max();
    pointCloud->maxZ = -std::numeric_limits<float>::max();

    for(const Point& point : pointCloud->cloud.points)
    {
        if(point.isFinite())
        {
            pointCloud->minZ = std::min(pointCloud->minZ, point.z);
            pointCloud->maxZ = std::max(pointCloud->maxZ, point.z);
        }
    }

    return std::unique_ptr<LoadResult>(new LoadResult{ std::move(pointCloud), std::move(result) });
}

bool Project::setLoadResult(LoadResult&& loadResult)
{
    if(! loadResult.pointCloud)
    {
        PCPS_LOG_ERROR << "Point cloud is null" << std::endl;
        return false;
    }

    if(! setResult(std::move(loadResult.result)))
    {
        PCPS_LOG_ERROR << "Result setup failed" << std::endl;
        return false;
    }

    _pointCloud = std::move(loadResult.pointCloud);
    return true;
}

std::unique_ptr<Result> Project::processCurrentPointCloud()
{
    if(! _pointCloud)
    {
        PCPS_LOG_ERROR << "There's no point cloud loaded" << std::endl;
        return nullptr;
    }

    std::unique_ptr<Result> result = _processPointCloud(*_pointCloud);

    if(! result)
    {
        PCPS_LOG_ERROR << "Loaded point cloud process failed" << std::endl;
        return nullptr;
    }

    return result;
}

bool Project::setResult(std::unique_ptr<Result>&& result) noexcept
{
    if(! result)
    {
        PCPS_LOG_ERROR << "Result is null" << std::endl;
        return false;
    }

    _result = std::move(result);
    return true;
}

std::unique_ptr<Result> Project::_processPointCloud(const PointCloud& pointCloud)
{
    if(! _context)
    {
        _context = Context::buildDefault();

        if(! _context)
        {
            PCPS_LOG_ERROR << "Context build failed" << std::endl;
            return nullptr;
        }
    }

    std::vector<Plane> planes;
    planeSegmentator.setStoreIntermediateResults(true);

    if(! planeSegmentator.segmentate(pointCloud.cloud, planes, *_context))
    {
        PCPS_LOG_ERROR << "Point cloud segmentation failed" << std::endl;
        return nullptr;
    }

    std::unique_ptr<Result> result(new Result());
    result->organizationResult = planeSegmentator.getLastOrganizationResult();
    result->organizationElapsedMcs = planeSegmentator.getLastOrganizationElapsedMcs();
    result->normalExtractionResult = planeSegmentator.getLastNormalExtractionResult();
    result->normalExtractionElapsedMcs = planeSegmentator.getLastNormalExtractionElapsedMcs();
    result->normalSplitResult = planeSegmentator.getLastNormalSplitResult();
    result->normalSplitElapsedMcs = planeSegmentator.getLastNormalSplitElapsedMcs();
    result->normalMergeResult = std::move(planes);
    result->normalMergeElapsedMcs = planeSegmentator.getLastNormalMergeElapsedMcs();
    return result;
}

}
}
