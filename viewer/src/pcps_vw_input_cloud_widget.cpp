/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_vw_input_cloud_widget.h"

#include "Q3DObserver"
#include "pcps_epsilon.h"
#include "pcps_vw_color_map.h"
#include "pcps_vw_point_cloud.h"

namespace pcps
{
namespace vw
{

QString InputCloudWidget::_updateViewer(const PointCloud* pointCloud, const Result*, Q3DObserver& viewer)
{
    QString text;

    if(pointCloud)
    {
        const std::vector<Point>& points = pointCloud->cloud.points;
        float minZ = pointCloud->minZ;
        float rangeZ = std::max(pointCloud->maxZ - minZ, epsilon);
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

        text += "Points: ";
        text += QString::number(numPoints);
    }

    return text;
}

}
}
