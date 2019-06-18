/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_thrust_normal_extractor.h"

#include "thrust/transform.h"
#include "thrust/device_vector.h"
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
    using DeviceVector = thrust::device_vector<float4>;


    struct NormalMeanTransform
    {
        DeviceVector::const_iterator normalsBegin;
        int normalRegionX;
        int normalRegionY;
        int normalRegionWidth;
        int normalsWidth;

        __device__
        float4 operator()(int index) const noexcept
        {
            // C = A % B is equivalent to C = A – B * (A / B)
            int aDivB = index / normalRegionWidth;
            // int aModB = index % normalRegionWidth;
            int aModB = index - (aDivB * normalRegionWidth);
            int indexX = normalRegionX + aModB;
            int indexY = normalRegionY + aDivB;
            int globalIndex = (indexY * normalsWidth) + indexX;
            return *(normalsBegin + globalIndex);
        }
    };


    struct NormalMeanReduce
    {
        __device__
        float4 operator()(float4 a, float4 b) const noexcept
        {
            return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
        }
    };


    struct NormalStdDevTransform
    {
        float4 mean;
        DeviceVector::const_iterator normalsBegin;
        int normalRegionX;
        int normalRegionY;
        int normalRegionWidth;
        int normalsWidth;

        __device__
        float operator()(int index) const noexcept
        {
            // C = A % B is equivalent to C = A – B * (A / B)
            int aDivB = index / normalRegionWidth;
            // int aModB = index % normalRegionWidth;
            int aModB = index - (aDivB * normalRegionWidth);
            int indexX = normalRegionX + aModB;
            int indexY = normalRegionY + aDivB;
            int globalIndex = (indexY * normalsWidth) + indexX;
            float4 normal = *(normalsBegin + globalIndex);
            float stdDev;

            if(normal.w > 0)
            {
                float dotProduct = normal.x * mean.x + normal.y * mean.y + normal.z * mean.z;
                stdDev = std::acos(dotProduct);
            }
            else
            {
                stdDev = 0;
            }

            return stdDev;
        }
    };
}

#pragma pop

void getNormalMean(const void* normals, int normalRegionX, int normalRegionY, int normalRegionWidth,
                   int normalRegionHeight, int normalsWidth, float4& mean, int& numValidNormals,
                   CachedAllocator& allocator)
{
    auto par = thrust::cuda::par(allocator);
    auto normalVector = static_cast<const DeviceVector*>(normals);
    thrust::counting_iterator<int> first(0);
    thrust::counting_iterator<int> last = first + (normalRegionWidth * normalRegionHeight);
    NormalMeanTransform transform{ normalVector->begin(), normalRegionX, normalRegionY, normalRegionWidth,
                normalsWidth };
    float4 sum = thrust::transform_reduce(par, first, last, transform, float4{ 0, 0, 0, 0 }, NormalMeanReduce());
    numValidNormals = int(std::round(sum.w));

    if(numValidNormals)
    {
        mean = { sum.x / numValidNormals, sum.y / numValidNormals, sum.z / numValidNormals, 1 };
    }
    else
    {
        mean = { 0, 0, 0, 0 };
    }
}

void getNormalStdDev(const void* normals, int normalRegionX, int normalRegionY, int normalRegionWidth,
                     int normalRegionHeight, int normalsWidth, float4 mean, int numValidNormals, float& stdDev,
                     CachedAllocator& allocator)
{
    auto par = thrust::cuda::par(allocator);
    auto normalVector = static_cast<const DeviceVector*>(normals);
    thrust::counting_iterator<int> first(0);
    thrust::counting_iterator<int> last = first + (normalRegionWidth * normalRegionHeight);
    NormalStdDevTransform transform{ mean, normalVector->begin(), normalRegionX, normalRegionY, normalRegionWidth,
                normalsWidth };
    float sum = thrust::transform_reduce(par, first, last, transform, 0.0f, thrust::plus<float>());
    stdDev = sum / numValidNormals;
}

}
