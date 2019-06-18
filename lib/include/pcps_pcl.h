/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#ifndef PCPS_PCL_H
#define PCPS_PCL_H

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include "pcps_cloud.h"

namespace pcps
{

/**
 * @brief Transform a PCL cloud into a pcps one.
 * @param pclCloud Input PCL cloud.
 * @param pcpsCloud Output pcps cloud.
 */
static inline void fromPCL(const pcl::PointCloud<pcl::PointXYZ>& pclCloud, Cloud& pcpsCloud)
{
    const Eigen::Vector4f& pclSensorOrigin = pclCloud.sensor_origin_;
    pcpsCloud.sensorOrigin = { pclSensorOrigin.x(), pclSensorOrigin.y(), pclSensorOrigin.z(), pclSensorOrigin.w() };
    pcpsCloud.width = int(pclCloud.width);
    pcpsCloud.height = int(pclCloud.height);

    const auto& pclPoints = pclCloud.points;
    std::vector<Point>& pcpsPoints = pcpsCloud.points;
    pcpsPoints.clear();

    if(std::size_t numPoints = pclPoints.size())
    {
        pcpsPoints.resize(numPoints);

        for(std::size_t index = 0; index < numPoints; ++index)
        {
            const pcl::PointXYZ& pclPoint = pclPoints[index];
            pcpsPoints[index] = { pclPoint.x, pclPoint.y, pclPoint.z, 0 };
        }
    }
}

/**
 * @brief Transform a pcps cloud into a PCL one.
 * @param pcpsCloud Input pcps cloud.
 * @param pclCloud Output PCL cloud.
 */
static inline void toPCL(const Cloud& pcpsCloud, pcl::PointCloud<pcl::PointXYZ>& pclCloud)
{
    const Point& pcpsSensorOrigin = pcpsCloud.sensorOrigin;
    pclCloud.sensor_origin_ = { pcpsSensorOrigin.x, pcpsSensorOrigin.y, pcpsSensorOrigin.z, 0 };
    pclCloud.width = std::uint32_t(pcpsCloud.width);
    pclCloud.height = std::uint32_t(pcpsCloud.height);

    const std::vector<Point>& pcpsPoints = pcpsCloud.points;
    auto& pclPoints = pclCloud.points;
    pclPoints.clear();

    if(std::size_t numPoints = pcpsPoints.size())
    {
        pclPoints.resize(numPoints);

        for(std::size_t index = 0; index < numPoints; ++index)
        {
            const Point& pcpsPoint = pcpsPoints[index];
            pclPoints[index] = { pcpsPoint.x, pcpsPoint.y, pcpsPoint.z };
        }
    }
}

}

#endif
