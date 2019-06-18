/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_normal_splitter.h"

#include "pcps_cloud.h"
#include "pcps_logger.h"
#include "pcps_context.h"
#include "pcps_epsilon.h"
#include "pcps_tweak_me.h"
#include "pcps_device_cloud.h"
#include "pcps_normal_region.h"
#include "pcps_boost_compute.h"

namespace bpc = boost::compute;
using DeviceView = bpc::mapped_view<bpc::float4_>;

namespace pcps
{

bool NormalSplitter::_getMean(const DeviceCloud& normalDeviceCloud, const NormalRegion& normalRegion, Point& mean,
                              int& numValidNormals, Context& context) const
{
    const Cloud& normalCloud = normalDeviceCloud.getHostCloud();

    if(normalRegion.width * normalRegion.height < PCPS_OPENCL_SPLITTER_MAX_CPU_REGION_AREA)
    {
        _getCpuMean(normalCloud, normalRegion, mean, numValidNormals);
    }
    else
    {
        auto deviceNormalsPtr = static_cast<const DeviceView*>(normalDeviceCloud.getDeviceData());
        const DeviceView& deviceNormals = *deviceNormalsPtr;
        int x = normalRegion.x;
        int y = normalRegion.y;
        int roiWidth = normalRegion.width;
        int globalWidth = normalCloud.width;
        std::array<int, 4> args = { x, y, roiWidth, globalWidth };
        bpc::mapped_view<int> deviceArgs(args.data(), args.size(), *context.context);

        BOOST_COMPUTE_CLOSURE(bpc::float4_, transform, (int index), (deviceNormals, deviceArgs),
        {
            int x = deviceArgs[0];
            int y = deviceArgs[1];
            int roiWidth = deviceArgs[2];
            int globalWidth = deviceArgs[3];

            // C = A % B is equivalent to C = A – B * (A / B)
            int aDivB = index / roiWidth;
            // int aModB = index % roiWidth;
            int aModB = index - (aDivB * roiWidth);
            int indexX = x + aModB;
            int indexY = y + aDivB;
            int globalIndex = (indexY * globalWidth) + indexX;
            return deviceNormals[globalIndex];
        });

        bpc::counting_iterator<int> first(0);
        bpc::counting_iterator<int> last = first + (normalRegion.width * normalRegion.height);
        bpc::float4_ sum = { 0, 0, 0, 0 };

        try
        {
            bpc::transform_reduce(first, last, &sum, transform, bpc::plus<bpc::float4_>(), *context.queue);
        }
        catch(const bpc::program_build_failure& programBuildFailure)
        {
            PCPS_LOG_ERROR << "Program build failure: " << std::endl;
            PCPS_LOG_ERROR << programBuildFailure.build_log() << std::endl;
            return false;
        }

        numValidNormals = int(std::round(sum[3]));

        if(numValidNormals)
        {
            mean = { sum[0] / numValidNormals, sum[1] / numValidNormals, sum[2] / numValidNormals, 1 };
        }
        else
        {
            mean = { 0, 0, 0, 0 };
        }
    }

    return true;
}

bool NormalSplitter::_getStdDev(const DeviceCloud& normalDeviceCloud, const NormalRegion& normalRegion,
                                const Point& mean, int numValidNormals, float& stdDev, Context& context) const
{
    const Cloud& normalCloud = normalDeviceCloud.getHostCloud();

    if(normalRegion.width * normalRegion.height < PCPS_OPENCL_SPLITTER_MAX_CPU_REGION_AREA)
    {
        _getCpuStdDev(normalCloud, normalRegion, mean, numValidNormals, stdDev);
    }
    else
    {
        auto deviceNormalsPtr = static_cast<const DeviceView*>(normalDeviceCloud.getDeviceData());
        const DeviceView& deviceNormals = *deviceNormalsPtr;
        auto bpcMean = reinterpret_cast<const bpc::float4_&>(mean);
        int x = normalRegion.x;
        int y = normalRegion.y;
        int roiWidth = normalRegion.width;
        int globalWidth = normalCloud.width;
        std::array<int, 4> intArgs = { x, y, roiWidth, globalWidth };
        bpc::mapped_view<int> deviceIntArgs(intArgs.data(), intArgs.size(), *context.context);
        bpc::mapped_view<bpc::float4_> deviceMean(&bpcMean, 1, *context.context);

        BOOST_COMPUTE_CLOSURE(float, transform, (int index), (deviceNormals, deviceIntArgs, deviceMean),
        {
            int x = deviceIntArgs[0];
            int y = deviceIntArgs[1];
            int roiWidth = deviceIntArgs[2];
            int globalWidth = deviceIntArgs[3];
            float4 mean = deviceMean[0];

            // C = A % B is equivalent to C = A – B * (A / B)
            int aDivB = index / roiWidth;
            // int aModB = index % roiWidth;
            int aModB = index - (aDivB * roiWidth);
            int indexX = x + aModB;
            int indexY = y + aDivB;
            int globalIndex = (indexY * globalWidth) + indexX;
            float4 normal = deviceNormals[globalIndex];
            float stdDev;

            if(normal.w > 0)
            {
                float dotProduct = normal.x * mean.x + normal.y * mean.y + normal.z * mean.z;
                stdDev = acos(dotProduct);
            }
            else
            {
                stdDev = 0;
            }

            return stdDev;
        });

        bpc::counting_iterator<int> first(0);
        bpc::counting_iterator<int> last = first + (normalRegion.width * normalRegion.height);
        float sum = 0;

        try
        {
            bpc::transform_reduce(first, last, &sum, transform, bpc::plus<float>(), *context.queue);
        }
        catch(const bpc::program_build_failure& programBuildFailure)
        {
            PCPS_LOG_ERROR << "Program build failure: " << std::endl;
            PCPS_LOG_ERROR << programBuildFailure.build_log() << std::endl;
            return false;
        }

        stdDev = sum / numValidNormals;
    }

    return true;
}

}
