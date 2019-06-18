/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_normal_splitter.h"

#include <cmath>
#include "pcps_cloud.h"
#include "pcps_logger.h"
#include "pcps_epsilon.h"
#include "pcps_device_cloud.h"
#include "pcps_normal_region.h"

namespace pcps
{

int NormalSplitter::getMinimumRegionWidth() const noexcept
{
    return _minimumRegionWidth;
}

bool NormalSplitter::setMinimumRegionWidth(int minimumRegionWidth) noexcept
{
    if(minimumRegionWidth < 1)
    {
        PCPS_LOG_ERROR << "Invalid minimumRegionWidth: " << minimumRegionWidth << std::endl;
        return false;
    }

    _minimumRegionWidth = minimumRegionWidth;
    return true;
}

int NormalSplitter::getMinimumRegionHeight() const noexcept
{
    return _minimumRegionHeight;
}

bool NormalSplitter::setMinimumRegionHeight(int minimumRegionHeight) noexcept
{
    if(minimumRegionHeight < 1)
    {
        PCPS_LOG_ERROR << "Invalid minimumRegionHeight: " << minimumRegionHeight << std::endl;
        return false;
    }

    _minimumRegionHeight = minimumRegionHeight;
    return true;
}

float NormalSplitter::getStdDsvThreshold() const noexcept
{
    return _stdDsvThreshold;
}

bool NormalSplitter::setStdDsvThreshold(float stdDsvThreshold) noexcept
{
    if(stdDsvThreshold <= epsilon)
    {
        PCPS_LOG_ERROR << "Invalid stdDsvThreshold: " << stdDsvThreshold << std::endl;
        return false;
    }

    _stdDsvThreshold = stdDsvThreshold;
    return true;
}

bool NormalSplitter::split(const Cloud& inputNormalCloud, std::vector<NormalRegion>& outputNormalRegions,
                           Context& context) const
{
    outputNormalRegions.clear();

    if(inputNormalCloud.points.empty())
    {
        PCPS_LOG_ERROR << "Normal cloud is empty" << std::endl;
        return false;
    }

    if(! inputNormalCloud.hasValidSize())
    {
        PCPS_LOG_ERROR << "Normal cloud size is invalid" << std::endl;
        return false;
    }

    DeviceCloud inputNormalDeviceCloud(inputNormalCloud, context);
    NormalRegion initialNormalRegion{ Point(), 0, 0, inputNormalCloud.width, inputNormalCloud.height };

    if(! _split(inputNormalDeviceCloud, initialNormalRegion, outputNormalRegions, context))
    {
        PCPS_LOG_ERROR << "Initial normal region split failed" << std::endl;
        return false;
    }

    if(outputNormalRegions.empty())
    {
        outputNormalRegions.push_back(initialNormalRegion);
    }

    return true;
}

bool NormalSplitter::split(const DeviceCloud& inputNormalDeviceCloud, std::vector<NormalRegion>& outputNormalRegions,
                           Context& context) const
{
    const Cloud& inputNormalCloud = inputNormalDeviceCloud.getHostCloud();
    outputNormalRegions.clear();

    if(inputNormalCloud.points.empty())
    {
        PCPS_LOG_ERROR << "Normal cloud is empty" << std::endl;
        return false;
    }

    if(! inputNormalCloud.hasValidSize())
    {
        PCPS_LOG_ERROR << "Normal cloud size is invalid" << std::endl;
        return false;
    }

    NormalRegion initialNormalRegion{ Point(), 0, 0, inputNormalCloud.width, inputNormalCloud.height };

    if(! _split(inputNormalDeviceCloud, initialNormalRegion, outputNormalRegions, context))
    {
        PCPS_LOG_ERROR << "Initial normal region split failed" << std::endl;
        return false;
    }

    if(outputNormalRegions.empty())
    {
        outputNormalRegions.push_back(initialNormalRegion);
    }

    return true;
}

bool NormalSplitter::_split(const DeviceCloud& inputNormalDeviceCloud, const NormalRegion& normalRegion,
                            std::vector<NormalRegion>& outputNormalRegions, Context& context) const
{
    if(normalRegion.width >= _minimumRegionWidth && normalRegion.height >= _minimumRegionHeight)
    {
        Point mean;
        int numValidNormals;

        if(! _getMean(inputNormalDeviceCloud, normalRegion, mean, numValidNormals, context))
        {
            PCPS_LOG_ERROR << "Mean calculation failed" << std::endl;
            return false;
        }

        if(numValidNormals)
        {
            float stdDev = 0;

            if(! _getStdDev(inputNormalDeviceCloud, normalRegion, mean, numValidNormals, stdDev, context))
            {
                PCPS_LOG_ERROR << "Standard deviation calculation failed" << std::endl;
                return false;
            }

            if(stdDev >= _stdDsvThreshold)
            {
                if(normalRegion.width > normalRegion.height)
                {
                    int halfWidth = normalRegion.width / 2;
                    NormalRegion leftNormalRegion{ Point(), normalRegion.x, normalRegion.y, halfWidth,
                                normalRegion.height };

                    if(! _split(inputNormalDeviceCloud, leftNormalRegion, outputNormalRegions, context))
                    {
                        PCPS_LOG_ERROR << "Left normal region split failed" << std::endl;
                        return false;
                    }

                    NormalRegion rightNormalRegion{ Point(), normalRegion.x + halfWidth, normalRegion.y,
                                normalRegion.width - halfWidth, normalRegion.height };

                    if(! _split(inputNormalDeviceCloud, rightNormalRegion, outputNormalRegions, context))
                    {
                        PCPS_LOG_ERROR << "Right normal region split failed" << std::endl;
                        return false;
                    }
                }
                else
                {
                    int halfHeight = normalRegion.height / 2;
                    NormalRegion upNormalRegion{ Point(), normalRegion.x, normalRegion.y, normalRegion.width,
                                halfHeight };

                    if(! _split(inputNormalDeviceCloud, upNormalRegion, outputNormalRegions, context))
                    {
                        PCPS_LOG_ERROR << "Up normal region split failed" << std::endl;
                        return false;
                    }

                    NormalRegion downNormalRegion{ Point(), normalRegion.x, normalRegion.y + halfHeight,
                                normalRegion.width, normalRegion.height - halfHeight };

                    if(! _split(inputNormalDeviceCloud, downNormalRegion, outputNormalRegions, context))
                    {
                        PCPS_LOG_ERROR << "Down normal region split failed" << std::endl;
                        return false;
                    }
                }
            }
            else
            {
                NormalRegion newNormalRegion = normalRegion;
                newNormalRegion.normal = mean;
                outputNormalRegions.push_back(newNormalRegion);
            }
        }
    }

    return true;
}

void NormalSplitter::_getCpuMean(const Cloud& normalCloud, const NormalRegion& normalRegion, Point& mean,
                                 int& numValidNormals) const
{
    const Point* normals = normalCloud.points.data();
    int cloudWidth = normalCloud.width;
    int xi = normalRegion.x;
    int xl = normalRegion.x + normalRegion.width;
    Point sum = { 0, 0, 0, 0 };

    for(int y = normalRegion.y, yl = normalRegion.y + normalRegion.height; y < yl; ++y)
    {
        const Point* normalsRow = normals + (y * cloudWidth);

        for(int x = xi; x < xl; ++x)
        {
            const Point& normal = normalsRow[x];
            sum.x += normal.x;
            sum.y += normal.y;
            sum.z += normal.z;
            sum.aux += normal.aux;
        }
    }

    numValidNormals = int(std::round(sum.aux));

    if(numValidNormals)
    {
        mean = { sum.x / numValidNormals, sum.y / numValidNormals, sum.z / numValidNormals, 1 };
    }
    else
    {
        mean = sum;
    }
}

void NormalSplitter::_getCpuStdDev(const Cloud& normalCloud, const NormalRegion& normalRegion, const Point& mean,
                                   int numValidNormals, float& stdDev) const
{
    const Point* normals = normalCloud.points.data();
    int cloudWidth = normalCloud.width;
    int xi = normalRegion.x;
    int xl = normalRegion.x + normalRegion.width;
    float result = 0;

    for(int y = normalRegion.y, yl = normalRegion.y + normalRegion.height; y < yl; ++y)
    {
        const Point* normalsRow = normals + (y * cloudWidth);

        for(int x = xi; x < xl; ++x)
        {
            const Point& normal = normalsRow[x];

            if(normal.aux > 0)
            {
                float dotProduct = normal.x * mean.x + normal.y * mean.y + normal.z * mean.z;
                result += std::acos(dotProduct);
            }
        }
    }

    stdDev = result / numValidNormals;
}

}
