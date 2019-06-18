/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_normal_merger.h"

#include <cmath>
#include "pcps_plane.h"
#include "pcps_logger.h"
#include "pcps_epsilon.h"

namespace pcps
{

float NormalMerger::getMinimumStdDsvThreshold() const noexcept
{
    return _minimumStdDsvThreshold;
}

float NormalMerger::getMaximumStdDsvThreshold() const noexcept
{
    return _maximumStdDsvThreshold;
}

bool NormalMerger::setStdDsvThresholds(float minimumStdDsvThreshold, float maximumStdDsvThreshold) noexcept
{
    if(minimumStdDsvThreshold <= epsilon)
    {
        PCPS_LOG_ERROR << "Invalid minimumStdDsvThreshold: " << minimumStdDsvThreshold << std::endl;
        return false;
    }

    if(maximumStdDsvThreshold <= minimumStdDsvThreshold)
    {
        PCPS_LOG_ERROR << "Invalid maximumStdDsvThreshold: " << maximumStdDsvThreshold << std::endl;
        return false;
    }

    _minimumStdDsvThreshold = minimumStdDsvThreshold;
    _maximumStdDsvThreshold = maximumStdDsvThreshold;
    return true;
}

void NormalMerger::merge(const std::vector<NormalRegion>& normalRegions, std::vector<Plane>& planes)
{
    std::size_t numNormalRegions = normalRegions.size();

    if(numNormalRegions == 0)
    {
        planes.clear();
        return;
    }

    if(numNormalRegions == 1)
    {
        planes.resize(1);
        planes[0].regions.resize(1, normalRegions[0]);
        return;
    }

    _setupGroups(normalRegions);

    for(std::size_t index = 0, limit = _groups.size(); index < limit; ++index)
    {
        std::size_t bestNeighborIndex;

        while(_getBestNeighborGroupIndex(index, bestNeighborIndex))
        {
            _mergeGroups(index, bestNeighborIndex);
        }
    }

    _buildPlanes(normalRegions, planes);
}

void NormalMerger::_setupGroups(const std::vector<NormalRegion>& normalRegions)
{
    int width = 0;
    int height = 0;

    for(const NormalRegion& normalRegion : normalRegions)
    {
        width = std::max(width, normalRegion.x + normalRegion.width);
        height = std::max(height, normalRegion.y + normalRegion.height);
    }

    ++width;
    ++height;
    _pointCloudIds.clear();
    _pointCloudIds.resize(std::size_t(width * height), 0);

    int* pointCloudIdsData = _pointCloudIds.data();
    std::size_t numNormalRegions = normalRegions.size();
    _groups.resize(numNormalRegions);

    for(std::size_t index = 0; index < numNormalRegions; ++index)
    {
        const NormalRegion& normalRegion = normalRegions[index];
        Group& group = _groups[index];
        group.regionIndexes.clear();
        group.regionIndexes.push_back(index);
        group.neighborGroupIndexes.clear();
        group.normal = normalRegion.normal;
        group.area = normalRegion.width * normalRegion.height;

        int id = int(index) + 1;
        int xi = normalRegion.x;
        int xl = normalRegion.x + normalRegion.width;
        int yi = normalRegion.y;
        int yl = normalRegion.y + normalRegion.height;

        for(int y = yi; y < yl; ++y)
        {
            int* pointCloudIdsRow = pointCloudIdsData + (y * width);

            for(int x = xi; x < xl; ++x)
            {
                pointCloudIdsRow[x] = id;
            }
        }
    }

    Group* groupsData = _groups.data();

    for(int y = 0; y < height - 1; ++y)
    {
        const int* pointCloudIdsRow = pointCloudIdsData + (y * width);
        const int* downPointCloudIdsRow = pointCloudIdsData + ((y + 1) * width);

        for(int x = 0; x < width - 1; ++x)
        {
            if(int a = pointCloudIdsRow[x])
            {
                if(int b = pointCloudIdsRow[x + 1])
                {
                    if(a != b)
                    {
                        groupsData[a - 1].neighborGroupIndexes.insert(std::size_t(b - 1));
                        groupsData[b - 1].neighborGroupIndexes.insert(std::size_t(a - 1));
                    }
                }

                if(int b = downPointCloudIdsRow[x])
                {
                    if(a != b)
                    {
                        groupsData[a - 1].neighborGroupIndexes.insert(std::size_t(b - 1));
                        groupsData[b - 1].neighborGroupIndexes.insert(std::size_t(a - 1));
                    }
                }
            }
        }
    }
}

bool NormalMerger::_getBestNeighborGroupIndex(std::size_t groupIndex, std::size_t& bestNeighborGroupIndex) const noexcept
{
    const Group& group = _groups[groupIndex];

    if(! group.area)
    {
        return false;
    }

    const Point& normal = group.normal;
    float minimumStdDsvThreshold = _minimumStdDsvThreshold;
    float bestStdDsv = _maximumStdDsvThreshold;
    bool neighborFound = false;

    for(std::size_t neighborGroupIndex : group.neighborGroupIndexes)
    {
        const Group& neighborGroup = _groups[neighborGroupIndex];
        const Point& neighborNormal = neighborGroup.normal;
        float dotProduct = normal.x * neighborNormal.x + normal.y * neighborNormal.y + normal.z * neighborNormal.z;
        float stdDsv = std::acos(dotProduct);

        if(stdDsv < bestStdDsv)
        {
            bestNeighborGroupIndex = neighborGroupIndex;
            bestStdDsv = stdDsv;
            neighborFound = true;

            if(stdDsv < minimumStdDsvThreshold)
            {
                break;
            }
        }
    }

    return neighborFound;
}

void NormalMerger::_mergeGroups(std::size_t aIndex, std::size_t bIndex)
{
    Group& aGroup = _groups[aIndex];
    Group& bGroup = _groups[bIndex];
    std::vector<std::size_t>& aRegionIndexes = aGroup.regionIndexes;
    const std::vector<std::size_t>& bRegionIndexes = bGroup.regionIndexes;
    aRegionIndexes.insert(aRegionIndexes.end(), bRegionIndexes.begin(), bRegionIndexes.end());

    Point& aNormal = aGroup.normal;
    const Point& bNormal = bGroup.normal;
    int aArea = aGroup.area;
    int bArea = bGroup.area;
    int areaSum = aArea + bArea;
    aNormal.x = (aNormal.x * aArea + bNormal.x * bArea) / areaSum;
    aNormal.y = (aNormal.y * aArea + bNormal.y * bArea) / areaSum;
    aNormal.z = (aNormal.z * aArea + bNormal.z * bArea) / areaSum;
    aGroup.area = areaSum;
    bGroup.area = 0;

    std::unordered_set<std::size_t>& aNeighbors = aGroup.neighborGroupIndexes;
    aNeighbors.erase(bIndex);

    for(std::size_t bNeighborIndex : bGroup.neighborGroupIndexes)
    {
        if(bNeighborIndex != aIndex)
        {
            aNeighbors.insert(bNeighborIndex);

            Group& bNeighborGroup = _groups[bNeighborIndex];
            std::unordered_set<std::size_t>& bNeighborNeighbors = bNeighborGroup.neighborGroupIndexes;
            bNeighborNeighbors.erase(bIndex);
            bNeighborNeighbors.insert(aIndex);
        }
    }
}

void NormalMerger::_buildPlanes(const std::vector<NormalRegion>& normalRegions, std::vector<Plane>& planes) const
{
    std::size_t planeIndex = 0;
    planes.resize(_groups.size());

    for(const Group& group : _groups)
    {
        if(group.area)
        {
            Plane& plane = planes[planeIndex];
            std::vector<NormalRegion>& planeRegions = plane.regions;
            planeRegions.clear();
            planeRegions.reserve(group.regionIndexes.size());
            ++planeIndex;

            for(std::size_t regionIndex : group.regionIndexes)
            {
                planeRegions.push_back(normalRegions[regionIndex]);
            }
        }
    }

    planes.resize(planeIndex);
}

}
