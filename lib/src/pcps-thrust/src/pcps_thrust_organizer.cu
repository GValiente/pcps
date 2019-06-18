/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_thrust_organizer.h"

#include <cmath>
#include "thrust/transform.h"
#include "thrust/device_vector.h"
#include "thrust/transform_reduce.h"
#include "thrust/iterator/counting_iterator.h"
#include "pcps_thrust_cached_allocator.h"

namespace pcps_thrust
{

// suppress "function was declared but never referenced warning"
// https://stackoverflow.com/questions/49836419/how-to-hide-nvccs-function-was-declared-but-never-referenced-warnings
#pragma push
#pragma diag_suppress 177

namespace
{
    using PointIterator = thrust::device_vector<float2>::const_iterator;

    struct MinMaxTransform
    {
        __device__
        float4 operator()(float2 point) const noexcept
        {
            return { point.x, point.y, point.x, point.y };
        }
    };

    struct MinMaxReduce
    {
        __device__
        float4 operator()(float4 a, float4 b) const noexcept
        {
            float minX = a.x < b.x ? a.x : b.x;
            float minY = a.y < b.y ? a.y : b.y;
            float maxX = a.z > b.z ? a.z : b.z;
            float maxY = a.w > b.w ? a.w : b.w;
            return { minX, minY, maxX, maxY };
        }
    };

    struct DistanceTransform
    {
        PointIterator pointsBegin;
        int numPoints;
        float maxDistance;

        __device__
        float operator()(int index) const noexcept
        {
            PointIterator pointsIt = pointsBegin + index;
            float2 point = *pointsIt;
            float minDistance = maxDistance;
            ++index;
            ++pointsIt;

            while(index < numPoints)
            {
                float2 otherPoint = *pointsIt;
                float distanceX = point.x - otherPoint.x;
                float distanceY = point.y - otherPoint.y;
                float distance = (distanceX * distanceX) + (distanceY * distanceY);

                if(distance < minDistance)
                {
                    minDistance = distance;
                }

                ++index;
                ++pointsIt;
            }

            return minDistance;
        }
    };

    struct DistanceReduce
    {
        __device__
        float operator()(float a, float b) const noexcept
        {
            return a < b ? a : b;
        }
    };

    struct IndexTransform
    {
        float minX;
        float minY;
        int width;
        float cellSizeInv;

        __device__
        int operator()(float2 point) const noexcept
        {
            float relativeX = point.x - minX;
            float relativeY = point.y - minY;
            auto col = int(relativeX * cellSizeInv);
            auto row = int(relativeY * cellSizeInv);
            return (row * width) + col;
        }
    };
}

#pragma pop

void organize(const thrust::host_vector<float2>& points2D, int maximumWidth, int maximumHeight, float epsilon,
              int& width, int& height, thrust::host_vector<int>& indices, CachedAllocator& allocator)
{
    auto par = thrust::cuda::par(allocator);
    thrust::device_vector<float2> devicePoints2D = points2D;
    std::size_t numPoints = points2D.size();

    // Retrieve bounding box:
    auto maxValue = std::numeric_limits<float>::max();
    float4 initialBoundingBox = { maxValue, maxValue, -maxValue, -maxValue };
    float4 boundingBox = thrust::transform_reduce(par, devicePoints2D.begin(), devicePoints2D.end(), MinMaxTransform(),
                                                  initialBoundingBox, MinMaxReduce());

    // Retrieve minimum distance between points:

    float minX = boundingBox.x;
    float minY = boundingBox.y;
    float distanceX = boundingBox.z - minX;
    float distanceY = boundingBox.w - minY;
    thrust::counting_iterator<int> first(0);
    thrust::counting_iterator<int> last = first + long(numPoints);
    float minDistance = thrust::transform_reduce(par, first, last,
                                                 DistanceTransform{ devicePoints2D.begin(), int(numPoints), maxValue },
                                                 maxValue, DistanceReduce());
    minDistance = std::sqrt(minDistance);

    // Retrieve output cloud size and cell size:

    if(minDistance > epsilon)
    {
        width = std::min(int(distanceX / minDistance) + 1, maximumWidth);
        height = std::min(int(distanceY / minDistance) + 1, maximumHeight);
    }
    else
    {
        width = maximumWidth;
        height = maximumHeight;
    }

    float cellSize = std::max(distanceX / width, distanceY / height);
    float cellSizeInv = 1 / cellSize;
    width = int(distanceX * cellSizeInv) + 1;
    height = int(distanceY * cellSizeInv) + 1;

    // Fill output cloud points vector:

    thrust::device_vector<int> deviceIndices(numPoints);
    thrust::transform(par, devicePoints2D.begin(), devicePoints2D.end(), deviceIndices.begin(),
                      IndexTransform{ minX, minY, width, cellSizeInv});
    indices.clear();
    indices.reserve(numPoints);
    thrust::copy(deviceIndices.begin(), deviceIndices.end(), indices.begin());
}

}
