/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_organization_cloud_widget.h"

#include "Q3DObserver"
#include "pcps_epsilon.h"
#include "pcps_vw_result.h"
#include "pcps_vw_color_map.h"
#include "pcps_vw_point_cloud.h"

namespace pcps
{
namespace vw
{

QString OrganizationCloudWidget::_updateViewer(const PointCloud* pointCloud, const Result* result, Q3DObserver& viewer)
{
    QString text;

    if(pointCloud && result)
    {
        float minZ = pointCloud->minZ;
        float rangeZ = std::max(pointCloud->maxZ - minZ, epsilon);

        const Cloud& cloud = result->organizationResult;
        const std::vector<Point>& points = cloud.points;
        int numPoints = 0;
        viewer.reservePoints(int(points.size()));

        for(const Point& point : points)
        {
            if(point.isFinite())
            {
                QVector3D position(point.x, point.y, point.z);
                float colorValue = (point.z - minZ) / rangeZ;
                viewer.addPoint(position, ColorMap::get(colorValue));
                ++numPoints;
            }
        }

        text += "Width: ";
        text += QString::number(cloud.width);
        text += "\nHeight: ";
        text += QString::number(cloud.height);
        text += "\nPoints: ";
        text += QString::number(numPoints);
        text += "\nElapsed microseconds: ";
        text += QString::number(result->organizationElapsedMcs);
    }

    return text;
}

}
}
